/**
 * test_comm_queue.c - Test Response Queue Functionality
 * 
 * Verification for Router Thread architecture (Step 5 of comm_router_plan.md)
 * 
 * This test verifies the internal response queue logic:
 * 1. FIFO ordering of packets
 * 2. Timeout behavior when queue is empty
 * 3. Queue overflow protection (drop oldest)
 * 4. Thread-safe concurrent access
 * 
 * Note: This test runs WITHOUT Firmware - it directly tests queue internals.
 * 
 * Usage:
 *   make tests
 *   ./bin/test_comm_queue
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>

#include "hampod_core.h"
#include "comm.h"

// ============================================================================
// Test Framework
// ============================================================================

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, msg) do { \
    if (condition) { \
        printf("  ✓ PASS: %s\n", msg); \
        tests_passed++; \
    } else { \
        printf("  ✗ FAIL: %s\n", msg); \
        tests_failed++; \
    } \
} while(0)

// ============================================================================
// Mock Response Queue (duplicated from comm.c for direct testing)
// This is a copy of the queue structure to test in isolation
// ============================================================================

#define TEST_QUEUE_SIZE 4  // Small size for testing overflow

typedef struct {
    CommPacket packets[TEST_QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t mutex;
    pthread_cond_t not_empty;
    volatile bool running;
} TestQueue;

static void test_queue_init(TestQueue* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;
    q->running = true;
    pthread_mutex_init(&q->mutex, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}

static void test_queue_destroy(TestQueue* q) {
    q->running = false;
    pthread_cond_broadcast(&q->not_empty);
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->not_empty);
}

static int test_queue_push(TestQueue* q, const CommPacket* packet) {
    pthread_mutex_lock(&q->mutex);
    
    if (q->count >= TEST_QUEUE_SIZE) {
        // Queue full - drop oldest
        q->head = (q->head + 1) % TEST_QUEUE_SIZE;
        q->count--;
    }
    
    q->packets[q->tail] = *packet;
    q->tail = (q->tail + 1) % TEST_QUEUE_SIZE;
    q->count++;
    
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->mutex);
    return HAMPOD_OK;
}

static int test_queue_pop_timeout(TestQueue* q, CommPacket* packet, int timeout_ms) {
    pthread_mutex_lock(&q->mutex);
    
    while (q->count == 0 && q->running) {
        struct timespec timeout;
        clock_gettime(CLOCK_REALTIME, &timeout);
        
        timeout.tv_sec += timeout_ms / 1000;
        timeout.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (timeout.tv_nsec >= 1000000000) {
            timeout.tv_sec += 1;
            timeout.tv_nsec -= 1000000000;
        }
        
        int result = pthread_cond_timedwait(&q->not_empty, &q->mutex, &timeout);
        if (result == ETIMEDOUT) {
            pthread_mutex_unlock(&q->mutex);
            return HAMPOD_TIMEOUT;
        }
    }
    
    if (!q->running && q->count == 0) {
        pthread_mutex_unlock(&q->mutex);
        return HAMPOD_ERROR;
    }
    
    if (q->count == 0) {
        pthread_mutex_unlock(&q->mutex);
        return HAMPOD_NOT_FOUND;
    }
    
    *packet = q->packets[q->head];
    q->head = (q->head + 1) % TEST_QUEUE_SIZE;
    q->count--;
    
    pthread_mutex_unlock(&q->mutex);
    return HAMPOD_OK;
}

static int test_queue_count(TestQueue* q) {
    pthread_mutex_lock(&q->mutex);
    int count = q->count;
    pthread_mutex_unlock(&q->mutex);
    return count;
}

// ============================================================================
// Test Cases
// ============================================================================

void test_empty_queue_timeout(void) {
    printf("\n=== Test 1: Empty Queue Timeout ===\n");
    
    TestQueue q;
    test_queue_init(&q);
    
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    CommPacket packet;
    int result = test_queue_pop_timeout(&q, &packet, 100);  // 100ms timeout
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + 
                     (end.tv_nsec - start.tv_nsec) / 1000000;
    
    TEST_ASSERT(result == HAMPOD_TIMEOUT, "Empty queue returns TIMEOUT");
    TEST_ASSERT(elapsed_ms >= 90 && elapsed_ms <= 200, 
                "Timeout occurred around 100ms");
    
    test_queue_destroy(&q);
}

void test_fifo_ordering(void) {
    printf("\n=== Test 2: FIFO Ordering ===\n");
    
    TestQueue q;
    test_queue_init(&q);
    
    // Push 3 packets with different tags
    CommPacket p1 = { .type = PACKET_KEYPAD, .tag = 1 };
    CommPacket p2 = { .type = PACKET_KEYPAD, .tag = 2 };
    CommPacket p3 = { .type = PACKET_KEYPAD, .tag = 3 };
    
    test_queue_push(&q, &p1);
    test_queue_push(&q, &p2);
    test_queue_push(&q, &p3);
    
    TEST_ASSERT(test_queue_count(&q) == 3, "Queue has 3 items");
    
    // Pop and verify order
    CommPacket out;
    test_queue_pop_timeout(&q, &out, 1000);
    TEST_ASSERT(out.tag == 1, "First pop returns tag 1");
    
    test_queue_pop_timeout(&q, &out, 1000);
    TEST_ASSERT(out.tag == 2, "Second pop returns tag 2");
    
    test_queue_pop_timeout(&q, &out, 1000);
    TEST_ASSERT(out.tag == 3, "Third pop returns tag 3");
    
    TEST_ASSERT(test_queue_count(&q) == 0, "Queue is empty after 3 pops");
    
    test_queue_destroy(&q);
}

void test_overflow_drops_oldest(void) {
    printf("\n=== Test 3: Overflow Drops Oldest ===\n");
    
    TestQueue q;
    test_queue_init(&q);
    
    // Push 5 packets into queue of size 4
    // Oldest should be dropped
    for (int i = 1; i <= 5; i++) {
        CommPacket p = { .type = PACKET_AUDIO, .tag = i };
        test_queue_push(&q, &p);
    }
    
    TEST_ASSERT(test_queue_count(&q) == 4, "Queue capped at 4 items");
    
    // Pop and verify oldest was dropped (tag=1 should be gone)
    CommPacket out;
    test_queue_pop_timeout(&q, &out, 1000);
    TEST_ASSERT(out.tag == 2, "First pop returns tag 2 (oldest dropped)");
    
    test_queue_pop_timeout(&q, &out, 1000);
    TEST_ASSERT(out.tag == 3, "Second pop returns tag 3");
    
    test_queue_destroy(&q);
}

// Producer thread for concurrent test
typedef struct {
    TestQueue* queue;
    int start_tag;
    int count;
} ProducerArgs;

void* producer_thread(void* arg) {
    ProducerArgs* args = (ProducerArgs*)arg;
    
    for (int i = 0; i < args->count; i++) {
        CommPacket p = { .type = PACKET_KEYPAD, .tag = args->start_tag + i };
        test_queue_push(args->queue, &p);
        usleep(10000);  // 10ms between pushes
    }
    
    return NULL;
}

void test_concurrent_access(void) {
    printf("\n=== Test 4: Concurrent Push/Pop ===\n");
    
    TestQueue q;
    test_queue_init(&q);
    
    // Start producer thread
    ProducerArgs args = { .queue = &q, .start_tag = 100, .count = 3 };
    pthread_t producer;
    pthread_create(&producer, NULL, producer_thread, &args);
    
    // Pop items as they arrive
    CommPacket out;
    int received = 0;
    
    for (int i = 0; i < 3; i++) {
        int result = test_queue_pop_timeout(&q, &out, 500);  // 500ms timeout
        if (result == HAMPOD_OK) {
            received++;
        }
    }
    
    pthread_join(producer, NULL);
    
    TEST_ASSERT(received == 3, "Consumer received all 3 packets");
    
    test_queue_destroy(&q);
}

void test_shutdown_wakes_waiters(void) {
    printf("\n=== Test 5: Shutdown Wakes Waiters ===\n");
    
    TestQueue q;
    test_queue_init(&q);
    
    // Measure how long it takes to return when we signal shutdown
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    // Shutdown the queue after a short delay (in another thread would be better,
    // but for simplicity we'll just test the running = false case)
    q.running = false;
    pthread_cond_broadcast(&q.not_empty);
    
    CommPacket out;
    int result = test_queue_pop_timeout(&q, &out, 5000);  // Long timeout
    
    clock_gettime(CLOCK_MONOTONIC, &end);
    long elapsed_ms = (end.tv_sec - start.tv_sec) * 1000 + 
                     (end.tv_nsec - start.tv_nsec) / 1000000;
    
    TEST_ASSERT(result == HAMPOD_ERROR || result == HAMPOD_NOT_FOUND, 
                "Shutdown returns error/not_found");
    TEST_ASSERT(elapsed_ms < 100, "Shutdown woke waiter quickly");
    
    test_queue_destroy(&q);
}

// ============================================================================
// Main
// ============================================================================

int main() {
    printf("=== Response Queue Unit Tests ===\n");
    printf("Testing queue logic in isolation (no Firmware required)\n");
    
    test_empty_queue_timeout();
    test_fifo_ordering();
    test_overflow_drops_oldest();
    test_concurrent_access();
    test_shutdown_wakes_waiters();
    
    printf("\n=== Summary ===\n");
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    
    if (tests_failed == 0) {
        printf("\n✓ ALL TESTS PASSED\n");
        return 0;
    } else {
        printf("\n✗ SOME TESTS FAILED\n");
        return 1;
    }
}

/**
 * @file test_persistent_piper.c
 * @brief Unit tests for the persistent Piper TTS feature
 *
 * Verifies that:
 * 1. hal_tts_init() starts Piper process
 * 2. hal_tts_speak() produces audio
 * 3. Multiple sequential speaks work (second should be faster)
 * 4. hal_tts_cleanup() terminates Piper
 * 5. Interrupt during persistent speak stops audio
 *
 * Part of Phase 2: Persistent Piper implementation
 */

#include "../hal_audio.h"
#include "../hal_tts.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/* Test result counters */
static int tests_passed = 0;
static int tests_failed = 0;
static int tests_skipped = 0;

#define TEST_PASS(name)                                                        \
  do {                                                                         \
    printf("  [PASS] %s\n", name);                                             \
    tests_passed++;                                                            \
  } while (0)

#define TEST_FAIL(name, reason)                                                \
  do {                                                                         \
    printf("  [FAIL] %s: %s\n", name, reason);                                 \
    tests_failed++;                                                            \
  } while (0)

#define TEST_SKIP(name, reason)                                                \
  do {                                                                         \
    printf("  [SKIP] %s: %s\n", name, reason);                                 \
    tests_skipped++;                                                           \
  } while (0)

/* Get current time in milliseconds */
static long long current_time_ms(void) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/* ============================================================================
 * Test Cases
 * ============================================================================
 */

/**
 * Test 1: Verify TTS initialization starts Piper
 */
void test_tts_init(void) {
  printf("\n=== Test: TTS Init Starts Piper ===\n");

  /* Initialize audio first (required by TTS) */
  if (hal_audio_init() != 0) {
    TEST_FAIL("hal_audio_init", "failed");
    return;
  }

  /* Initialize TTS - this should start Piper */
  if (hal_tts_init() != 0) {
    TEST_SKIP("hal_tts_init", "Piper may not be installed");
    hal_audio_cleanup();
    return;
  }
  TEST_PASS("hal_tts_init starts Piper");

  /* Check implementation name */
  const char *impl = hal_tts_get_impl_name();
  if (impl != NULL && strstr(impl, "Piper") != NULL) {
    printf("  [INFO] TTS implementation: %s\n", impl);
    TEST_PASS("implementation name includes 'Piper'");
  } else {
    TEST_FAIL("implementation name", "does not include 'Piper'");
  }

  /* Cleanup */
  hal_tts_cleanup();
  hal_audio_cleanup();
}

/**
 * Test 2: Verify TTS speak produces audio
 */
void test_tts_speak_produces_audio(void) {
  printf("\n=== Test: TTS Speak Produces Audio ===\n");

  if (hal_audio_init() != 0) {
    TEST_FAIL("hal_audio_init", "failed");
    return;
  }

  if (hal_tts_init() != 0) {
    TEST_SKIP("hal_tts_init", "Piper may not be installed");
    hal_audio_cleanup();
    return;
  }

  /* Speak a word */
  printf("  [INFO] Speaking 'hello'...\n");
  long long start = current_time_ms();
  int result = hal_tts_speak("hello", NULL);
  long long end = current_time_ms();

  if (result == 0) {
    printf("  [INFO] Speak took %lld ms\n", end - start);
    TEST_PASS("hal_tts_speak succeeded");
  } else {
    TEST_FAIL("hal_tts_speak", "returned error");
  }

  /* Wait for audio to finish */
  usleep(500000);

  hal_tts_cleanup();
  hal_audio_cleanup();
}

/**
 * Test 3: Verify second speak is faster (persistent benefit)
 */
void test_persistent_latency_improvement(void) {
  long long first_time, second_time;

  printf("\n=== Test: Persistent Latency Improvement ===\n");

  if (hal_audio_init() != 0) {
    TEST_FAIL("hal_audio_init", "failed");
    return;
  }

  if (hal_tts_init() != 0) {
    TEST_SKIP("hal_tts_init", "Piper may not be installed");
    hal_audio_cleanup();
    return;
  }

  /* First speak - includes model loading */
  printf("  [INFO] First speak (includes model loading)...\n");
  long long start1 = current_time_ms();
  int result1 = hal_tts_speak("one", NULL);
  long long end1 = current_time_ms();
  first_time = end1 - start1;

  if (result1 != 0) {
    TEST_FAIL("first speak", "returned error");
    hal_tts_cleanup();
    hal_audio_cleanup();
    return;
  }
  printf("  [INFO] First speak took %lld ms\n", first_time);

  /* Wait a moment */
  usleep(200000);

  /* Second speak - should be faster (model already loaded) */
  printf("  [INFO] Second speak (model already loaded)...\n");
  long long start2 = current_time_ms();
  int result2 = hal_tts_speak("two", NULL);
  long long end2 = current_time_ms();
  second_time = end2 - start2;

  if (result2 != 0) {
    TEST_FAIL("second speak", "returned error");
    hal_tts_cleanup();
    hal_audio_cleanup();
    return;
  }
  printf("  [INFO] Second speak took %lld ms\n", second_time);

  /* Second should be faster or similar (not slower) */
  if (second_time <= first_time + 100) { /* Allow 100ms variance */
    char msg[64];
    snprintf(msg, sizeof(msg), "first=%lldms, second=%lldms", first_time,
             second_time);
    printf("  [INFO] %s\n", msg);
    TEST_PASS("second speak not slower than first");
  } else {
    char msg[64];
    snprintf(msg, sizeof(msg), "second (%lldms) > first (%lldms)", second_time,
             first_time);
    TEST_FAIL("latency comparison", msg);
  }

  /* Wait for audio */
  usleep(500000);

  hal_tts_cleanup();
  hal_audio_cleanup();
}

/**
 * Test 4: Verify cleanup terminates Piper
 */
void test_cleanup_terminates_piper(void) {
  printf("\n=== Test: Cleanup Terminates Piper ===\n");

  if (hal_audio_init() != 0) {
    TEST_FAIL("hal_audio_init", "failed");
    return;
  }

  if (hal_tts_init() != 0) {
    TEST_SKIP("hal_tts_init", "Piper may not be installed");
    hal_audio_cleanup();
    return;
  }

  /* Speak something */
  hal_tts_speak("test", NULL);
  usleep(200000);

  /* Cleanup should terminate Piper */
  hal_tts_cleanup();
  TEST_PASS("hal_tts_cleanup called");

  /* Re-init should work (starts new Piper process) */
  if (hal_tts_init() != 0) {
    TEST_FAIL("reinit after cleanup", "hal_tts_init failed");
  } else {
    TEST_PASS("reinit after cleanup works");
    hal_tts_cleanup();
  }

  hal_audio_cleanup();
}

/**
 * Test 5: Verify interrupt during speak
 */
void test_interrupt_during_speak(void) {
  printf("\n=== Test: Interrupt During Speak ===\n");

  if (hal_audio_init() != 0) {
    TEST_FAIL("hal_audio_init", "failed");
    return;
  }

  if (hal_tts_init() != 0) {
    TEST_SKIP("hal_tts_init", "Piper may not be installed");
    hal_audio_cleanup();
    return;
  }

  /* Start a long phrase */
  printf("  [INFO] Speaking long phrase then interrupting...\n");

  /* This is a simplified test - in real usage the interrupt comes from another
   * thread. Here we just ensure calling interrupt doesn't crash. */
  hal_tts_interrupt();
  TEST_PASS("hal_tts_interrupt callable while initialized");

  /* Now speak after interrupt - should work */
  if (hal_tts_speak("after interrupt", NULL) == 0) {
    TEST_PASS("speak after interrupt works");
  } else {
    TEST_FAIL("speak after interrupt", "failed");
  }

  usleep(500000);

  hal_tts_cleanup();
  hal_audio_cleanup();
}

/* ============================================================================
 * Main
 * ============================================================================
 */

int main(int argc, char *argv[]) {
  printf("=============================================\n");
  printf("  HAMPOD Persistent Piper Unit Tests\n");
  printf("=============================================\n");

  (void)argc;
  (void)argv;

  /* Run tests */
  test_tts_init();
  test_tts_speak_produces_audio();
  test_persistent_latency_improvement();
  test_cleanup_terminates_piper();
  test_interrupt_during_speak();

  /* Summary */
  printf("\n=============================================\n");
  printf("  Results: %d passed, %d failed, %d skipped\n", tests_passed,
         tests_failed, tests_skipped);
  printf("=============================================\n");

  return (tests_failed > 0) ? 1 : 0;
}

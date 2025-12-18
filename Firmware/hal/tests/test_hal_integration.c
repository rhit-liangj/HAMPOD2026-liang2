/**
 * @file test_hal_integration.c
 * @brief Integration test for Keypad + TTS + Audio
 * 
 * This test verifies the complete loop:
 * 1. Read key from USB keypad (HAL)
 * 2. Convert key to word
 * 3. Speak the word using TTS HAL
 * 
 * Also tests key hold detection - if same key received multiple times
 * in a row, it's a held key.
 * 
 * Compile: make test_hal_integration
 * Run: ./test_hal_integration
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include "hal_keypad.h"
#include "hal_audio.h"
#include "hal_tts.h"

static volatile int running = 1;

void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("\nShutting down...\n");
        running = 0;
    }
}

/* Map key character to spoken word */
const char* get_spoken_word(char key) {
    switch(key) {
        case '0': return "zero";
        case '1': return "one";
        case '2': return "two";
        case '3': return "three";
        case '4': return "four";
        case '5': return "five";
        case '6': return "six";
        case '7': return "seven";
        case '8': return "eight";
        case '9': return "nine";
        case 'A': return "A";
        case 'B': return "B";
        case 'C': return "C";
        case 'D': return "D";
        case '*': return "star";
        case '#': return "pound";
        case 'X': return "num lock";
        case 'Y': return "backspace";
        default:  return "unknown";
    }
}

int main() {
    printf("=== HAMPOD Integration Test: Keypad + TTS + Audio ===\n");
    printf("Press Ctrl+C to exit\n");
    printf("Hold a key for 1+ seconds to hear 'key held'\n\n");
    
    /* Set up signal handler */
    signal(SIGINT, signal_handler);
    
    /* Initialize HALs */
    printf("Initializing Keypad HAL...\n");
    if (hal_keypad_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize keypad\n");
        return 1;
    }
    
    printf("Initializing Audio HAL...\n");
    if (hal_audio_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize audio\n");
        hal_keypad_cleanup();
        return 1;
    }
    
    printf("Initializing TTS HAL...\n");
    if (hal_tts_init() != 0) {
        fprintf(stderr, "ERROR: Failed to initialize TTS\n");
        hal_audio_cleanup();
        hal_keypad_cleanup();
        return 1;
    }
    printf("TTS Engine: %s\n", hal_tts_get_impl_name());
    
    printf("\nSystem Ready!\n");
    printf("Press any key on the keypad. The Pi should speak the key name.\n\n");
    
    /* Hold detection state */
    char last_key = '-';
    int hold_count = 0;
    int no_event_count = 0;   /* Consecutive polls with no event */
    int hold_spoken = 0;      /* Have we spoken "key held"? */
    const int HOLD_THRESHOLD = 100;   /* 100 polls @ 10ms = 1 second hold */
    const int RELEASE_THRESHOLD = 30; /* 30 consecutive no-events (300ms) = released */
    
    while (running) {
        KeypadEvent event = hal_keypad_read();
        
        if (event.valid) {
            no_event_count = 0;  /* Reset no-event counter */
            
            if (event.key == last_key) {
                /* Same key still held - increment hold counter */
                hold_count++;
                
                if (hold_count >= HOLD_THRESHOLD && !hold_spoken) {
                    printf("Key '%c' HELD for 1+ second\n", event.key);
                    hal_tts_speak("key held", NULL);
                    hold_spoken = 1;
                }
                /* Don't speak again - just continue holding */
            } else {
                /* New key press (different from last key) */
                const char* word = get_spoken_word(event.key);
                printf("Key: '%c' -> Speaking: \"%s\"\n", event.key, word);
                hal_tts_speak(word, NULL);
                
                last_key = event.key;
                hold_count = 1;
                hold_spoken = 0;
            }
        } else {
            /* No event this poll */
            if (last_key != '-') {
                /* Key was being held - increment both counters */
                no_event_count++;
                hold_count++;  /* Keep counting hold time even during gaps */
                
                /* Check for hold threshold even during no-event polls */
                if (hold_count >= HOLD_THRESHOLD && !hold_spoken) {
                    printf("Key '%c' HELD for 1+ second\n", last_key);
                    hal_tts_speak("key held", NULL);
                    hold_spoken = 1;
                }
                
                /* Only reset after several consecutive no-events */
                if (no_event_count >= RELEASE_THRESHOLD) {
                    last_key = '-';
                    hold_count = 0;
                    hold_spoken = 0;
                }
            }
        }
        
        /* Small delay to avoid CPU spinning */
        usleep(10000);  /* 10ms */
    }
    
    /* Cleanup */
    printf("\nCleaning up...\n");
    hal_tts_cleanup();
    hal_audio_cleanup();
    hal_keypad_cleanup();
    
    return 0;
}


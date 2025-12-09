/**
 * @file speech_latency_test.c
 * @brief Integration test for Keypad + Speech + Audio with Latency Measurement
 * 
 * Supports:
 * - Festival (via text2wave)
 * - Piper (via piper command line)
 * 
 * Compare latency of synthesis between the two engines.
 * 
 * Usage: ./speech_latency_test [festival|piper]
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>
#include "hal_keypad.h"
#include "hal_audio.h"

/* Default Piper Model - Update this path if needed */
#ifndef PIPER_MODEL_PATH
#define PIPER_MODEL_PATH "en_US-lessac-medium.onnx"
#endif

/* Default Piper Executable */
#ifndef PIPER_BIN
#define PIPER_BIN "piper"
#endif

/* Low Quality Model Path */
#ifndef PIPER_MODEL_LOW_PATH
#define PIPER_MODEL_LOW_PATH "en_US-lessac-low.onnx"
#endif

/* Speech Rate (Length Scale): Passed as argument. Default 1.0 */
/* #define PIPER_LENGTH_SCALE "0.75" */ 


typedef enum {
    ENGINE_FESTIVAL,
    ENGINE_PIPER,
    ENGINE_PIPER_LOW,
    ENGINE_PIPER_STREAM,
    ENGINE_PIPER_PERSISTENT
} SpeechEngine;

static volatile int running = 1;
static SpeechEngine current_engine = ENGINE_FESTIVAL;
static FILE *persistent_pipe = NULL;
static char current_length_scale[16] = "1.0"; /* Default to normal speed */

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
        case 'A': return "alpha";
        case 'B': return "bravo";
        case 'C': return "charlie";
        case 'D': return "delta";
        case '*': return "star";
        case '#': return "pound";
        case 'X': return "num lock";
        case 'Y': return "backspace";
        default:  return "unknown";
    }
}

/* Get current time in milliseconds */
long long current_timestamp_ms() {
    struct timeval te; 
    gettimeofday(&te, NULL);
    long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    return milliseconds;
}

int main(int argc, char *argv[]) {
    printf("=== HAMPOD Speech Latency Test ===\n");
    printf("Press Ctrl+C to exit\n\n");

    /* Parse arguments */
    if (argc > 1) {
        if (strcmp(argv[1], "piper") == 0) {
            current_engine = ENGINE_PIPER;
            printf("Selected Engine: PIPER (Model: %s)\n", PIPER_MODEL_PATH);
        } else if (strcmp(argv[1], "piper-low") == 0) {
            current_engine = ENGINE_PIPER_LOW;
            printf("Selected Engine: PIPER LOW (Model: %s)\n", PIPER_MODEL_LOW_PATH);
        } else if (strcmp(argv[1], "piper-stream") == 0) {
            current_engine = ENGINE_PIPER_STREAM;
            printf("Selected Engine: PIPER STREAM (Model: %s)\n", PIPER_MODEL_LOW_PATH);
        } else if (strcmp(argv[1], "piper-persistent") == 0) {
            current_engine = ENGINE_PIPER_PERSISTENT;
            printf("Selected Engine: PIPER PERSISTENT (Model: %s)\n", PIPER_MODEL_LOW_PATH);
        } else if (strcmp(argv[1], "festival") == 0) {
            current_engine = ENGINE_FESTIVAL;
            printf("Selected Engine: FESTIVAL\n");
        } else {
            printf("Unknown engine '%s'. Defaulting to FESTIVAL.\n", argv[1]);
        }
        
        /* Parse Speed Argument */
        if (argc > 2) {
            strncpy(current_length_scale, argv[2], sizeof(current_length_scale) - 1);
            current_length_scale[sizeof(current_length_scale)-1] = '\0';
            printf("Selected Speed (Length Scale): %s\n", current_length_scale);
        }
    } else {
        printf("Defaulting to FESTIVAL. Usage: %s [festival|piper|piper-low|piper-stream|piper-persistent] [speed]\n", argv[0]);
    }

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
    
    printf("\nSystem Ready!\n");
    printf("Press any key on the keypad. The Pi should speak the key name.\n\n");
    
    char command[1024];
    const char* output_file = "/tmp/hampod_speak.wav";

    /* Initialize Persistent Pipe if selected */
    if (current_engine == ENGINE_PIPER_PERSISTENT) {
        /* Construct the pipeline: Piper -> Aplay */
        /* Note: We force line buffering or rely on Piper's behavior */
        const char* audio_dev = hal_audio_get_device();
        snprintf(command, sizeof(command), "%s --model %s --length_scale %s --output_raw | aplay -D %s -r 16000 -f S16_LE -t raw -", 
                 PIPER_BIN, PIPER_MODEL_LOW_PATH, current_length_scale, audio_dev);
        
        printf("Starting persistent pipeline (Speed: %s)...\nCMD: %s\n", current_length_scale, command);
        persistent_pipe = popen(command, "w");
        
        if (persistent_pipe == NULL) {
            fprintf(stderr, "ERROR: Failed to start persistent pipeline.\n");
            return 1;
        }
        printf("Pipeline started. Speaking is now asynchronous.\n\n");
    }
    
    while (running) {
        KeypadEvent event = hal_keypad_read();
        
        if (event.valid) {
            const char* word = get_spoken_word(event.key);
            printf("Key: '%c' -> Speaking: \"%s\"\n", event.key, word);
            
            long long start_time = current_timestamp_ms();
            int ret = -1;

            if (current_engine == ENGINE_PIPER_PERSISTENT) {
                /* Persistent Mode: just write to the pipe */
                if (persistent_pipe) {
                    fprintf(persistent_pipe, "%s\n", word);
                    fflush(persistent_pipe);
                    ret = 0;
                }
            } else if (current_engine == ENGINE_FESTIVAL) {
                /* Festival: text2wave */
                snprintf(command, sizeof(command), "echo \"%s\" | text2wave -o %s", word, output_file);
                ret = system(command);
            } else if (current_engine == ENGINE_PIPER) {
                /* Piper: Medium Quality */
                snprintf(command, sizeof(command), "echo \"%s\" | %s --model %s --length_scale %s --output_file %s", 
                         word, PIPER_BIN, PIPER_MODEL_PATH, current_length_scale, output_file);
                ret = system(command);
            } else if (current_engine == ENGINE_PIPER_LOW) {
                /* Piper: Low Quality */
                snprintf(command, sizeof(command), "echo \"%s\" | %s --model %s --length_scale %s --output_file %s", 
                         word, PIPER_BIN, PIPER_MODEL_LOW_PATH, current_length_scale, output_file);
                ret = system(command);
            } else {
                /* Piper: Stream (Low Quality) */
                /* Output raw audio directly to aplay */
                /* Low quality models are typically 16000Hz */
                /* Use the HAL-detected device */
                const char* audio_dev = hal_audio_get_device();
                snprintf(command, sizeof(command), "echo \"%s\" | %s --model %s --length_scale %s --output_raw | aplay -D %s -r 16000 -f S16_LE -t raw -", 
                         word, PIPER_BIN, PIPER_MODEL_LOW_PATH, current_length_scale, audio_dev);
                ret = system(command);
            }

            long long end_time = current_timestamp_ms();
            long long latency = end_time - start_time;
            
            if (ret == 0) {
                if (current_engine == ENGINE_PIPER_STREAM) {
                   printf("Total Playback Time (Stream): %lld ms\n", latency);
                } else if (current_engine == ENGINE_PIPER_PERSISTENT) {
                   printf("Dispatch Time: %lld ms (Async)\n", latency);
                } else {
                   printf("Synthesis Latency: %lld ms\n", latency);
                   /* Play the generated file */
                   hal_audio_play_file(output_file);
                }
            } else {
                fprintf(stderr, "Error generating speech. Command failed: \n%s\n", command);
            }
        }

        
        /* Small delay to avoid CPU spinning */
        usleep(10000);  /* 10ms */
    }
    
    /* Cleanup */
    printf("\nCleaning up...\n");
    if (persistent_pipe) {
        printf("Closing persistent pipeline...\n");
        pclose(persistent_pipe);
    }
    hal_keypad_cleanup();
    hal_audio_cleanup();
    remove(output_file);
    
    return 0;
}

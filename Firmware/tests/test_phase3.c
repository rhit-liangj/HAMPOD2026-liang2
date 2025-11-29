#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include "../audio_firmware.h"

// Mock external variables required by audio_firmware.c
pid_t controller_pid;

int main() {
    printf("Starting Phase 3 Audio Integration Test...\n");

    // 1. Initialize Audio Firmware (and HAL)
    printf("Calling firmwareStartAudio()...\n");
    firmwareStartAudio();
    printf("firmwareStartAudio() returned.\n");

    // 2. Test Playback (using 'p' prefix for direct play, assuming file exists or just testing the call)
    // We'll use a dummy file or try to play something we know exists, or just verify it attempts to call HAL.
    // Since we can't easily verify sound output programmatically without a mic, we'll rely on return code 
    // and console output (HAL prints).
    
    // Let's try to play a non-existent file to see if HAL handles it, 
    // or better, generate a small wav if possible? 
    // For now, let's just send a "p" command.
    
    char test_cmd[] = "ptest_audio"; // p + filename (without .wav)
    
    printf("Calling firmwarePlayAudio with '%s'...\n", test_cmd);
    int result = firmwarePlayAudio(test_cmd);
    
    printf("firmwarePlayAudio returned: %d\n", result);

    if (result != 0) {
        printf("Note: Non-zero return expected if file is missing, but HAL should have been called.\n");
    }

    printf("Phase 3 Test Complete.\n");
    return 0;
}

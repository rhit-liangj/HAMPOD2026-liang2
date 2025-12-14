/**
 * test_compile.c - Verification that Software2 builds correctly
 * 
 * This is a placeholder test to verify the build system works.
 * Run: make tests
 * Execute: ./bin/test_compile
 */

#include <stdio.h>
#include "hampod_core.h"

int main() {
    printf("=== Software2 Build Verification ===\n");
    
    // Test KeyPressEvent struct
    KeyPressEvent event = {
        .key = '5',
        .shiftAmount = 0,
        .isHold = false
    };
    
    LOG_INFO("KeyPressEvent test: key='%c', shift=%d, hold=%s",
             event.key, event.shiftAmount, event.isHold ? "yes" : "no");
    
    printf("Build verification: SUCCESS\n");
    return 0;
}

/**
 * @file frequency_mode.c
 * @brief Frequency Entry Mode implementation
 * 
 * Part of Phase 1: Frequency Mode Implementation
 */

#include "frequency_mode.h"
#include "radio.h"
#include "speech.h"
#include "hampod_core.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// ============================================================================
// Module State
// ============================================================================

static FreqModeState g_state = FREQ_MODE_IDLE;
static VfoSelection g_selected_vfo = VFO_CURRENT;

// Frequency accumulator
#define MAX_FREQ_DIGITS 12
static char g_freq_buffer[MAX_FREQ_DIGITS + 1];
static int g_freq_len = 0;
static bool g_has_decimal = false;

// ============================================================================
// Internal Helpers
// ============================================================================

static const char* vfo_name(VfoSelection vfo) {
    switch (vfo) {
        case VFO_A:       return "VFO A";
        case VFO_B:       return "VFO B";
        case VFO_CURRENT: return "Current VFO";
        default:          return "VFO";
    }
}

static void clear_freq_buffer(void) {
    memset(g_freq_buffer, 0, sizeof(g_freq_buffer));
    g_freq_len = 0;
    g_has_decimal = false;
}

static void announce_digit(char digit) {
    char text[16];
    snprintf(text, sizeof(text), "%c", digit);
    speech_say_text(text);
}

static void announce_frequency(double freq_hz) {
    // Convert Hz to MHz and format for speech
    double freq_mhz = freq_hz / 1000000.0;
    
    // Format as "14 point 250 megahertz"
    char text[64];
    int mhz_part = (int)freq_mhz;
    int khz_part = (int)((freq_mhz - mhz_part) * 1000 + 0.5);
    
    if (khz_part == 0) {
        snprintf(text, sizeof(text), "%d megahertz", mhz_part);
    } else {
        snprintf(text, sizeof(text), "%d point %03d megahertz", mhz_part, khz_part);
    }
    
    speech_say_text(text);
}

static double parse_frequency(void) {
    // Parse the accumulated buffer as a frequency in MHz
    // Returns frequency in Hz
    
    if (g_freq_len == 0) {
        return -1.0;
    }
    
    double freq_mhz = atof(g_freq_buffer);
    
    // Validate reasonable range (100 kHz to 500 MHz)
    if (freq_mhz < 0.1 || freq_mhz > 500.0) {
        return -1.0;
    }
    
    return freq_mhz * 1000000.0;  // Convert to Hz
}

static void submit_frequency(void) {
    double freq_hz = parse_frequency();
    
    if (freq_hz < 0) {
        speech_say_text("Invalid frequency");
        clear_freq_buffer();
        g_state = FREQ_MODE_IDLE;
        return;
    }
    
    DEBUG_PRINT("submit_frequency: %.3f MHz\n", freq_hz / 1000000.0);
    
    // Set frequency on radio
    if (radio_set_frequency(freq_hz) == 0) {
        speech_say_text("Frequency set");
        announce_frequency(freq_hz);
    } else {
        speech_say_text("Failed to set frequency");
    }
    
    clear_freq_buffer();
    g_state = FREQ_MODE_IDLE;
}

// ============================================================================
// Initialization
// ============================================================================

void frequency_mode_init(void) {
    g_state = FREQ_MODE_IDLE;
    g_selected_vfo = VFO_CURRENT;
    clear_freq_buffer();
    DEBUG_PRINT("frequency_mode_init: Initialized\n");
}

// ============================================================================
// Key Handling
// ============================================================================

bool frequency_mode_handle_key(char key, bool is_hold) {
    (void)is_hold;  // Hold not used differently in this mode for now
    
    DEBUG_PRINT("frequency_mode_handle_key: key='%c' state=%d\n", key, g_state);
    
    switch (g_state) {
        case FREQ_MODE_IDLE:
            // Not in frequency mode - check for entry key
            if (key == '#') {
                // Enter frequency mode
                g_state = FREQ_MODE_SELECT_VFO;
                speech_say_text(vfo_name(g_selected_vfo));
                return true;
            }
            return false;  // Key not consumed
            
        case FREQ_MODE_SELECT_VFO:
            if (key == '#') {
                // Cycle VFO selection
                g_selected_vfo = (g_selected_vfo + 1) % 3;
                speech_say_text(vfo_name(g_selected_vfo));
                return true;
            }
            if (isdigit(key)) {
                // Start entering digits
                g_state = FREQ_MODE_ENTERING;
                g_freq_buffer[g_freq_len++] = key;
                g_freq_buffer[g_freq_len] = '\0';
                announce_digit(key);
                return true;
            }
            if (key == '*') {
                // Cancel
                clear_freq_buffer();
                g_state = FREQ_MODE_IDLE;
                speech_say_text("Cancelled");
                return true;
            }
            if (key == 'D') {
                // Clear/exit
                clear_freq_buffer();
                g_state = FREQ_MODE_IDLE;
                speech_say_text("Cancelled");
                return true;
            }
            return true;  // Consume but ignore other keys
            
        case FREQ_MODE_ENTERING:
            if (isdigit(key)) {
                // Accumulate digit
                if (g_freq_len < MAX_FREQ_DIGITS) {
                    g_freq_buffer[g_freq_len++] = key;
                    g_freq_buffer[g_freq_len] = '\0';
                    announce_digit(key);
                }
                return true;
            }
            if (key == '*') {
                if (!g_has_decimal) {
                    // Insert decimal point
                    if (g_freq_len < MAX_FREQ_DIGITS) {
                        g_freq_buffer[g_freq_len++] = '.';
                        g_freq_buffer[g_freq_len] = '\0';
                        g_has_decimal = true;
                        speech_say_text("point");
                    }
                } else {
                    // Second * cancels
                    clear_freq_buffer();
                    g_state = FREQ_MODE_IDLE;
                    speech_say_text("Cancelled");
                }
                return true;
            }
            if (key == '#') {
                // Submit frequency
                submit_frequency();
                return true;
            }
            if (key == 'D') {
                // Clear/exit
                clear_freq_buffer();
                g_state = FREQ_MODE_IDLE;
                speech_say_text("Cancelled");
                return true;
            }
            return true;  // Consume but ignore other keys
    }
    
    return false;
}

bool frequency_mode_is_active(void) {
    return g_state != FREQ_MODE_IDLE;
}

FreqModeState frequency_mode_get_state(void) {
    return g_state;
}

void frequency_mode_cancel(void) {
    if (g_state != FREQ_MODE_IDLE) {
        clear_freq_buffer();
        g_state = FREQ_MODE_IDLE;
        DEBUG_PRINT("frequency_mode_cancel: Cancelled\n");
    }
}

// ============================================================================
// Radio Polling Integration
// ============================================================================

void frequency_mode_on_radio_change(double new_freq) {
    // Announce frequency change from VFO dial
    // Only announce if NOT actively entering a frequency
    if (g_state == FREQ_MODE_IDLE) {
        DEBUG_PRINT("frequency_mode_on_radio_change: %.3f MHz\n", new_freq / 1000000.0);
        announce_frequency(new_freq);
    }
}

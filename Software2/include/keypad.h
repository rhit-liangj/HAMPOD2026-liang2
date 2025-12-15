/**
 * keypad.h - Keypad Input Module
 * 
 * Handles keypad input from Firmware with:
 * - Polling loop for key events
 * - Hold detection (long press)
 * - Callback system for key handlers
 * 
 * Usage:
 *   keypad_init();
 *   keypad_register_callback(my_handler);
 *   // Handler called when keys pressed/held
 *   keypad_shutdown();
 * 
 * Part of Phase 0: Core Infrastructure (Step 3.1)
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include "hampod_core.h"

// ============================================================================
// Callback Types
// ============================================================================

/**
 * Keypad event callback function type.
 * 
 * @param event Pointer to the key event (do not free)
 */
typedef void (*KeypadCallback)(const KeyPressEvent* event);

// ============================================================================
// Initialization & Cleanup
// ============================================================================

/**
 * Initialize the keypad system.
 * 
 * Starts a background thread that polls for keypad input.
 * Must be called after comm_init() and comm_wait_ready().
 * 
 * @return HAMPOD_OK on success, HAMPOD_ERROR on failure
 */
int keypad_init(void);

/**
 * Shutdown the keypad system.
 * 
 * Stops the polling thread and cleans up resources.
 */
void keypad_shutdown(void);

/**
 * Check if keypad system is running.
 * @return true if keypad thread is active
 */
bool keypad_is_running(void);

// ============================================================================
// Callback Registration
// ============================================================================

/**
 * Register a callback for keypad events.
 * 
 * The callback is invoked whenever a key is pressed or held.
 * Only one callback can be registered at a time.
 * 
 * @param callback Function to call on key events (NULL to unregister)
 */
void keypad_register_callback(KeypadCallback callback);

// ============================================================================
// Configuration
// ============================================================================

/**
 * Set the hold detection threshold.
 * 
 * If a key is held for longer than this duration, the event is marked
 * as a "hold" event (isHold = true).
 * 
 * Default: 500ms (HOLD_THRESHOLD_MS from hampod_core.h)
 * 
 * @param ms Threshold in milliseconds
 */
void keypad_set_hold_threshold(int ms);

/**
 * Set the polling interval.
 * 
 * How often to query the Firmware for key state.
 * Lower values = more responsive, but more CPU/pipe usage.
 * 
 * Default: 50ms
 * 
 * @param ms Polling interval in milliseconds
 */
void keypad_set_poll_interval(int ms);

#endif // KEYPAD_H

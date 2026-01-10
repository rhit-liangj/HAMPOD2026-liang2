# HAL Test Programs Makefile
# Compiles test programs for USB keypad, audio, and integration tests
# Last Updated: January 2026

CC = gcc
CFLAGS = -Wall -I.. -DUSE_PIPER -DBEEP_BASE_PATH=\"../../pregen_audio/\"
LDFLAGS = -lasound -lm
HAL_DIR = ..

# HAL source files
HAL_KEYPAD = $(HAL_DIR)/hal_keypad_usb.c
HAL_AUDIO = $(HAL_DIR)/hal_audio_usb.c
HAL_TTS = $(HAL_DIR)/hal_tts_piper.c
HAL_USB_UTIL = $(HAL_DIR)/hal_usb_util.c

# Test executables
TARGETS = test_hal_audio test_hal_usb_util test_hal_keypad test_hal_integration

.PHONY: all clean test

all: $(TARGETS)

# Audio HAL unit tests (automated)
test_hal_audio: test_hal_audio.c $(HAL_AUDIO) $(HAL_TTS) $(HAL_USB_UTIL)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: test_hal_audio"
	@echo "Run with: ./test_hal_audio"

# USB utility tests (automated)
test_hal_usb_util: test_hal_usb_util.c $(HAL_USB_UTIL)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built: test_hal_usb_util"
	@echo "Run with: ./test_hal_usb_util"

# Keypad HAL test (manual - waits for key presses)
test_hal_keypad: test_hal_keypad.c $(HAL_KEYPAD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: test_hal_keypad"
	@echo "Run with: sudo ./test_hal_keypad"
	@echo "NOTE: This test requires manual interaction (pressing keys)"

# Integration test: Keypad + Audio + TTS (manual - waits for key presses)
test_hal_integration: test_hal_integration.c $(HAL_KEYPAD) $(HAL_AUDIO) $(HAL_TTS) $(HAL_USB_UTIL)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: test_hal_integration"
	@echo "Run with: sudo ./test_hal_integration"
	@echo "NOTE: This test requires manual interaction (pressing keys)"

# Run automated tests only
test: test_hal_audio test_hal_usb_util
	@echo ""
	@echo "=== Running Automated HAL Tests ==="
	./test_hal_audio
	./test_hal_usb_util
	@echo ""
	@echo "=== Automated Tests Complete ==="
	@echo "For manual tests, run: sudo ./test_hal_keypad or sudo ./test_hal_integration"

clean:
	rm -f $(TARGETS)
	rm -f /tmp/hampod_test.wav /tmp/hampod_speak.wav

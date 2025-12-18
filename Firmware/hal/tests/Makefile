# HAL Test Programs Makefile
# Compiles test programs for USB keypad and audio HAL

CC = gcc
CFLAGS = -Wall -I.. -DUSE_PIPER
LDFLAGS = -lasound
HAL_DIR = ..

# HAL source files
HAL_KEYPAD = $(HAL_DIR)/hal_keypad_usb.c
HAL_AUDIO = $(HAL_DIR)/hal_audio_usb.c
HAL_TTS = $(HAL_DIR)/hal_tts_piper.c

# Test executables
TARGETS = test_hal_keypad test_hal_audio test_hal_integration

all: $(TARGETS)

test_hal_keypad: test_hal_keypad.c $(HAL_KEYPAD)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: test_hal_keypad"
	@echo "Run with: ./test_hal_keypad"

test_hal_audio: test_hal_audio.c $(HAL_AUDIO) $(HAL_TTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: test_hal_audio"
	@echo "Run with: ./test_hal_audio"

test_hal_integration: test_hal_integration.c $(HAL_KEYPAD) $(HAL_AUDIO) $(HAL_TTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: test_hal_integration"
	@echo "Run with: ./test_hal_integration"

clean:
	rm -f $(TARGETS)
	rm -f /tmp/hampod_test.wav /tmp/hampod_speak.wav

.PHONY: all clean


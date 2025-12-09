# Speech Comparison Makefile
# Compiles the latency test

CC = gcc
CFLAGS = -Wall -I../Firmware/hal -lm

HAL_DIR = ../Firmware/hal

TARGET = speech_latency_test

SRCS = speech_latency_test.c \
       $(HAL_DIR)/hal_keypad_usb.c \
       $(HAL_DIR)/hal_audio_usb.c

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) -o $@ $^ $(CFLAGS)
	@echo "Built: $(TARGET)"
	@echo "Run with: ./$(TARGET) [festival|piper]"

clean:
	rm -f $(TARGET)
	rm -f /tmp/hampod_speak.wav

.PHONY: all clean

# Hampod Firmware #

## Description ##

This section of the repo contains all of the code for the Firmware. The firmware runs on Raspberry Pi and provides hardware abstraction for USB keypad input and USB audio output. It communicates with the Software layer via named pipes.

**Platform:** Raspberry Pi (migrated from NanoPi)  
**Hardware:** USB numeric keypad (19 keys) + USB audio device  
**Architecture:** Hardware Abstraction Layer (HAL) for device independence

See [BUILD.md](BUILD.md) for detailed build instructions and dependencies.

## Communication ##

The Firmware idles until a command is sent to it via named pipes. The two pipes used for communication are `Firmware_i` and `Firmware_o` (from the Firmware's perspective: input and output).

Data is sent through the pipes in a packet format:

||Type|data length (bytes)|tag|data|
| :---: | :---: | :---: | :---: | :---: |
|Size (bytes)|4|2|2|0-65535|
|Description|The type of packet (keypad, audio, config)|Length of data field in bytes| Tag value for packet (0-65535)| Additional data (typically strings)|

## Structure ##

The firmware consists of three separate processes: the Firmware controller, keypad code, and audio code. Each process has two threads: one for packet encoding/decoding and one for performing actions. This design minimizes blocking.

Every packet sent to the Firmware receives a response packet (except during startup).

### Firmware controller ###

This code interfaces with the Software layer. It decodes packets to determine the destination process. `config` packets modify controller behavior. Invalid packets are discarded.

### Keypad ###

This code reads the USB keypad via the Hardware Abstraction Layer (HAL) and returns the pressed key as a character ('0'-'9', 'A'-'D', '*', '#', etc.).

**USB Keypad (19 keys):**
- Numeric keys: 0-9
- Function keys: /, *, -, +
- Special keys: ENTER, BACKSPACE, NUM_LOCK, etc.
- Returns '-' if no key or multiple keys pressed

A properly formatted packet contains a lowercase `r` in the data field. Additional data is ignored.

### Audio ###

This code handles audio playback on the HAMPOD using the HAL for USB audio output. It supports:
- **Text-to-speech**: Prefix with `s` (e.g., `sHello World`)
- **WAV playback**: Prefix with `p` (e.g., `ppath/to/file/sound`)
- **Direct TTS**: Prefix with `d` (temporary file in `/tmp`)

The firmware automatically appends `.wav` to file paths and uses Festival for text-to-speech synthesis.

*Note: The Firmware will attempt to play files whether they exist or not, returning an error if the file is not found.*

## Packets Returned ##

During normal operation, the Firmware always sends a response packet. However, response order is not guaranteed - faster operations (like keypad reads) return before slower ones (like text-to-speech).

The `tag` field allows Software to track which response corresponds to which request, since the `type` field alone may not be sufficient.

## Building & Running ##

### Quick Start
```bash
make              # Build firmware.elf
make debug        # Build with debug symbols and print statements
make clean        # Clean build artifacts
```

### Dependencies
See [BUILD.md](BUILD.md) for complete dependency list. Key requirements:
- `libasound2-dev` (ALSA for USB audio)
- `festival` (text-to-speech)

### Debug Output
The debug version includes color-coded output for each thread/process with prefixes like `Audio - Main:` or `Keypad - IO:` to aid debugging.

## Hardware Abstraction Layer (HAL)

The firmware uses a HAL to support different hardware configurations:

- **Keypad HAL** (`hal/hal_keypad_usb.c`): Reads USB keypad via Linux input events
- **Audio HAL** (`hal/hal_audio_usb.c`): Plays audio via ALSA to USB device

This architecture allows easy adaptation to different hardware without changing the core firmware logic.

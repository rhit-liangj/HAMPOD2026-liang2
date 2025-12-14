# Software2 - HAMPOD Fresh Start Implementation

This is the new Software layer for HAMPOD2026, built with clean architecture following the "Fresh Start" approach.

## Status: Phase 0 (Core Infrastructure)

See [Phase Zero Plan](../Documentation/Project_Overview_and_Onboarding/fresh-start-phase-zero-plan.md) for details.

## Directory Structure

```
Software2/
├── include/           # Public headers
│   └── hampod_core.h  # Core types and constants
├── src/               # Source files
│   └── main.c         # Main entry point
├── tests/             # Test programs
│   └── test_compile.c # Build verification
├── bin/               # Output binaries (auto-created)
├── obj/               # Object files (auto-created)
├── Makefile           # Build system
└── README.md          # This file
```

## Building

```bash
# Build main executable
make

# Build tests
make tests

# Clean build artifacts
make clean
```

## Module Roadmap

| Module | File | Status | Description |
|--------|------|--------|-------------|
| Core | `hampod_core.h` | ✅ Done | Types, constants, debug macros |
| Comm | `comm.c` | ⏳ TODO | Pipe communication with Firmware |
| Speech | `speech.c` | ⏳ TODO | Non-blocking speech queue |
| Keypad | `keypad.c` | ⏳ TODO | Key event handling with hold detection |
| Config | `config.c` | ⏳ TODO | Save/load settings |

## Communication with Firmware

Software2 communicates with the Firmware via named pipes:

| Pipe | Direction | Purpose |
|------|-----------|---------|
| `Keypad_o` | Firmware → Software | Key press events |
| `Firmware_i` | Software → Firmware | Audio requests |
| `Firmware_o` | Firmware → Software | Status messages |

### Audio Packet Format

| Type | Example | Description |
|------|---------|-------------|
| `d` | `dHello World` | Speak text via TTS (Festival/Piper) |
| `p` | `p/path/file.wav` | Play WAV file |
| `s` | `sABC123` | Spell out characters |

## Dependencies

- GCC with pthread support
- ALSA libraries (on Pi)
- Firmware must be running first

## Testing on Raspberry Pi

1. Ensure Firmware is running: `cd ../Firmware && ./firmware.elf`
2. Build: `make`
3. Run: `./bin/hampod`

Or use the integrated test script:
```bash
../Documentation/scripts/deploy_and_run_imitation.ps1
```

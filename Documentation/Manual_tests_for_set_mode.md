# Manual Tests for HAMPOD Modes

This document contains manual test procedures for verifying the functionality of Normal Mode, Frequency Mode, and Set Mode on the HAMPOD device.

**Last Updated:** 2025-12-27

---

## Prerequisites

Before running these tests:

1. **Hardware Setup:**
   - Raspberry Pi connected to IC-7300 via USB cable
   - IC-7300 powered on with USB CI-V enabled
   - 16-key keypad connected to RPi GPIO
   - Audio output (speaker/headphones) connected

2. **Software Running:**
   - Firmware running: `cd ~/HAMPOD2026/Firmware && ./firmware.elf &`
   - Software running: `cd ~/HAMPOD2026/Software2 && ./bin/hampod`

3. **Verify Radio Connection:**
   - Check for `/dev/ttyUSB0`: `ls -la /dev/ttyUSB0`
   - Software should announce "Connected to radio" on startup

---

## Normal Mode Tests

Normal Mode is the default operating mode for basic radio operations.

### Test N1: Volume Control

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[C]` | Announces current volume level (e.g., "Volume 25") |
| 2 | Press `[C]` again | Volume increases by 5, announces new level |
| 3 | Hold `[C]` | Volume decreases, announces new level |
| 4 | Repeat until volume reaches 100 | Should wrap or stop at maximum |
| 5 | Repeat decrease until volume reaches 0 | Should stop at minimum (not go negative) |

### Test N2: Mode Cycling

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[D]` | Announces current radio mode (e.g., "USB", "LSB", "CW") |
| 2 | Press `[D]` again | Cycles to next mode, announces it |
| 3 | Continue pressing `[D]` | Should cycle through: USB → LSB → CW → CW-R → AM → FM → USB |

### Test N3: Query Functions

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[1]` | Announces current frequency (e.g., "14 point 0 7 4 0 0 megahertz") |
| 2 | Press `[2]` | Announces current mode (e.g., "USB") |
| 3 | Press `[3]` | Announces current S-meter reading (e.g., "S 7") |
| 4 | Press `[4]` | Announces current power level |

### Test N4: Verbosity Toggle

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[0]` | Toggles verbosity, announces "Announcements on" or "Announcements off" |
| 2 | With announcements off, turn VFO dial | No frequency announcements |
| 3 | Press `[0]` again | Announces "Announcements on" |
| 4 | Turn VFO dial | Frequency changes are announced |

### Test N5: PTT Function

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Hold `[A]` | Radio should key up (TX indicator on radio) |
| 2 | Release `[A]` | Radio should unkey (back to RX) |

---

## Frequency Mode Tests

Frequency Mode allows direct frequency entry via the keypad.

### Test F1: Entry and VFO Selection

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` | Announces "Frequency Mode" |
| 2 | Press `[#]` again | Announces "VFO A" |
| 3 | Press `[#]` again | Announces "VFO B" |
| 4 | Press `[#]` again | Announces "Current VFO" |
| 5 | Press `[*]` twice or `[D]` | Announces "Cancelled", exits to Normal Mode |

### Test F2: Manual Frequency Entry with Decimal

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` | Announces "Frequency Mode" |
| 2 | Press `[1]` | Announces "1" |
| 3 | Press `[4]` | Announces "4" |
| 4 | Press `[*]` | Announces "point" |
| 5 | Press `[0]` `[7]` `[4]` | Announces each digit |
| 6 | Press `[#]` | Sets frequency, announces "14 point 0 7 4 0 0 megahertz" |
| 7 | Verify on radio display | Should show 14.074.00 MHz |

### Test F3: Auto-Decimal Frequency Entry (4-5 digits)

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` | Announces "Frequency Mode" |
| 2 | Press `[1]` `[4]` `[0]` `[2]` `[5]` | Announces each digit (no decimal needed) |
| 3 | Press `[#]` | Auto-inserts decimal, announces "14 point 0 2 5 0 0 megahertz" |
| 4 | Verify on radio display | Should show 14.025.00 MHz |

**Additional Auto-Decimal Test Cases:**

| Input | Expected Frequency |
|-------|-------------------|
| `7074` | 7.074 MHz |
| `14074` | 14.074 MHz |
| `21200` | 21.200 MHz |
| `28400` | 28.400 MHz |

### Test F4: Frequency Entry Cancellation

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` | Announces "Frequency Mode" |
| 2 | Press `[1]` `[4]` | Announces digits |
| 3 | Press `[D]` | Announces "Cancelled", exits to Normal Mode |
| 4 | Verify on radio | Frequency unchanged |

### Test F5: Frequency Entry Timeout

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` | Announces "Frequency Mode" |
| 2 | Enter some digits | Announces digits |
| 3 | Wait 10+ seconds | Announces "Timeout", exits to Normal Mode |

### Test F6: Invalid Frequency Rejection

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` | Announces "Frequency Mode" |
| 2 | Press `[9]` `[9]` `[9]` | Announces digits |
| 3 | Press `[#]` | Announces "Invalid frequency" (999 MHz out of range) |

---

## Set Mode Tests

Set Mode allows adjustment of radio parameters like power, mic gain, noise blanker, etc.

### Test S1: Enter and Exit Set Mode

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[*]` | Announces "Set Mode cancelled", exits to Normal Mode |

### Test S2: Parameter Navigation

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[#]` | Announces "Mic Gain" |
| 3 | Press `[#]` | Announces "Compression" |
| 4 | Press `[#]` | Announces "Noise Blanker" |
| 5 | Press `[#]` | Announces "Noise Reduction" |
| 6 | Press `[#]` | Announces "AGC" |
| 7 | Press `[#]` | Announces "Preamp" |
| 8 | Press `[#]` | Announces "Attenuation" |
| 9 | Press `[#]` | Announces "Mode" (wraps back to Power on next press) |

### Test S3: Power Level Setting

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[5]` `[0]` | Announces each digit |
| 3 | Press `[#]` | Announces "Power set to 50" |
| 4 | Verify on radio | Power level should show 50W |

### Test S4: Noise Blanker Toggle

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[#]` three times | Navigate to "Noise Blanker" |
| 3 | Press `[1]` | Announces "Noise blanker on" or toggles NB |
| 4 | Press `[0]` | Announces "Noise blanker off" |
| 5 | Verify on radio | NB indicator should match setting |

### Test S5: Shift Key Functionality

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[A]` | Announces "Shift" (shift state active) |
| 3 | Press a shifted key | Performs shifted action |
| 4 | Shift state should auto-clear after one key |

### Test S6: Current Value Query in Set Mode

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[0]` | Announces current power level value |
| 3 | Press `[#]` to go to Mic Gain | Announces "Mic Gain" |
| 4 | Press `[0]` | Announces current mic gain value |

### Test S7: Value Accumulation and Confirmation

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[B]` | Announces "Set Mode, Power" |
| 2 | Press `[1]` `[0]` `[0]` | Announces each digit |
| 3 | Press `[#]` | Announces "Power set to 100" |
| 4 | Press `[#]` to go to Mic Gain | Announces "Mic Gain" |
| 5 | Press `[5]` `[0]` | Announces each digit |
| 6 | Press `[*]` | Announces "Cancelled" (clears buffer, stays in Set Mode) |
| 7 | Press `[*]` again | Exits Set Mode entirely |

---

## Integration Tests

These tests verify that modes work together correctly.

### Test I1: Mode Transitions

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Start in Normal Mode | Basic key functions work |
| 2 | Press `[#]` | Enter Frequency Mode |
| 3 | Press `[*]` twice | Exit back to Normal Mode |
| 4 | Press `[B]` | Enter Set Mode |
| 5 | Press `[*]` | Exit back to Normal Mode |
| 6 | Press `[#]` | Enter Frequency Mode |
| 7 | Press `[D]` | Exit back to Normal Mode |

### Test I2: Mode Priority

| Step | Action | Expected Result |
|------|--------|-----------------|
| 1 | Press `[#]` to enter Frequency Mode | In Frequency Mode |
| 2 | Press `[B]` | Should be ignored (Frequency Mode active) |
| 3 | Press `[D]` to cancel | Back to Normal Mode |
| 4 | Press `[B]` to enter Set Mode | In Set Mode |
| 5 | Press `[#]` | Should navigate parameters (not enter Freq Mode) |

### Test I3: Radio Connection Required Tests

⚠️ **Note:** These tests require the radio to be connected (`/dev/ttyUSB0` present).

| Test | Action | Expected Result |
|------|--------|-----------------|
| Query Frequency | Press `[1]` in Normal Mode | Current frequency announced |
| Set Frequency | Enter frequency in Freq Mode | Radio tunes to frequency |
| Set Power | Set power in Set Mode | Radio power level changes |

---

## Troubleshooting

### Radio Not Connected

If tests requiring radio fail:

1. Check USB connection: `ls -la /dev/ttyUSB0`
2. Check radio power and CI-V USB setting
3. Check software log: `tail -f /tmp/hampod.log`

### No Audio Output

1. Check speaker/headphone connection
2. Verify volume: Press `[C]` to check/adjust
3. Check ALSA: `aplay -l` to list audio devices

### No Keypad Response

1. Check GPIO connections
2. Verify Firmware is running: `ps aux | grep firmware`
3. Check pipes exist: `ls -la ~/HAMPOD2026/Firmware/Firmware_*`

---

## Test Results Log

Use this section to record test results:

| Date | Tester | Test ID | Pass/Fail | Notes |
|------|--------|---------|-----------|-------|
| | | | | |
| | | | | |
| | | | | |

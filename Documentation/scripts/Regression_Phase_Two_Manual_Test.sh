#!/bin/bash
# =============================================================================
# HAMPOD2026 Manual Test: Phase 2 Normal Mode
# =============================================================================
# This script sets up and runs the normal mode for manual testing:
#   - Cleans up stale processes and pipes
#   - Builds Firmware and Software2
#   - Starts Firmware
#   - Starts Software2 hampod binary for manual testing
#
# Manual verification steps:
#   - Press [2] to hear current frequency
#   - Press [1] to select VFO A and hear frequency
#   - Hold [1] to select VFO B and hear frequency
#   - Press [0] to hear current mode (USB, LSB, etc.)
#   - Press [*] to hear S-meter reading
#   - Hold [*] to hear power level
#   - Press [C] to toggle announcements on/off
#
# Usage: ./Regression_Phase_Two_Manual_Test.sh
#
# Prerequisites:
#   - USB keypad connected
#   - USB audio device connected
#   - Radio connected via USB (for queries)
#   - Piper TTS model installed in Firmware/models/
#
# =============================================================================

set -e  # Exit on error during setup

# Configuration
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
REPO_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
FIRMWARE_DIR="$REPO_ROOT/Firmware"
SOFTWARE2_DIR="$REPO_ROOT/Software2"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m' # No Color

echo -e "${CYAN}======================================================${NC}"
echo -e "${CYAN}  HAMPOD2026 Phase 2 - Normal Mode Manual Test        ${NC}"
echo -e "${CYAN}======================================================${NC}"
echo ""
echo "Firmware dir: $FIRMWARE_DIR"
echo "Software2 dir: $SOFTWARE2_DIR"
echo ""

# -----------------------------------------------------------------------------
# Step 1: Clean up stale processes
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 1/7] Cleaning up stale processes...${NC}"
sudo killall -9 firmware.elf 2>/dev/null || true
sudo killall -9 hampod 2>/dev/null || true
sudo killall -9 phase0_test 2>/dev/null || true
sudo killall -9 piper 2>/dev/null || true
sudo killall -9 aplay 2>/dev/null || true
sleep 1
echo "  Done."

# -----------------------------------------------------------------------------
# Step 2: Clean up stale pipes
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 2/7] Cleaning up stale pipes...${NC}"
sudo rm -f "$FIRMWARE_DIR/Firmware_i" "$FIRMWARE_DIR/Firmware_o" 2>/dev/null || true
sudo rm -f "$FIRMWARE_DIR/Speaker_i" "$FIRMWARE_DIR/Speaker_o" 2>/dev/null || true
sudo rm -f "$FIRMWARE_DIR/Keypad_o" 2>/dev/null || true
rm -f /tmp/hampod_output.txt /tmp/firmware.log 2>/dev/null || true
echo "  Done."

# -----------------------------------------------------------------------------
# Step 3: Verify hardware is connected
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 3/7] Verifying hardware...${NC}"

# Check for USB keypad
KEYPAD_DEVICE=$(ls /dev/input/by-id/*kbd* 2>/dev/null | head -1 || echo "")
if [ -z "$KEYPAD_DEVICE" ]; then
    echo -e "${RED}  ERROR: No USB keypad found!${NC}"
    exit 1
fi
echo -e "  ${GREEN}✓${NC} USB Keypad: $KEYPAD_DEVICE"

# Check for USB audio
AUDIO_DEVICE=$(aplay -l 2>/dev/null | grep -i "usb\|device" | head -1 || echo "")
if [ -z "$AUDIO_DEVICE" ]; then
    echo -e "${YELLOW}  WARNING: No USB audio device detected${NC}"
else
    echo -e "  ${GREEN}✓${NC} Audio: $AUDIO_DEVICE"
fi

# Check for USB serial (radio)
RADIO_DEVICE=$(ls /dev/ttyUSB* 2>/dev/null | head -1 || echo "")
if [ -z "$RADIO_DEVICE" ]; then
    echo -e "${YELLOW}  WARNING: No USB serial device (radio) found${NC}"
    echo -e "${YELLOW}           Normal mode will work but queries may fail${NC}"
else
    echo -e "  ${GREEN}✓${NC} Radio: $RADIO_DEVICE"
fi

# Check for Piper model
PIPER_MODEL="$FIRMWARE_DIR/models/en_US-lessac-low.onnx"
if [ ! -f "$PIPER_MODEL" ]; then
    echo -e "${RED}  ERROR: Piper model not found at $PIPER_MODEL${NC}"
    exit 1
fi
echo -e "  ${GREEN}✓${NC} Piper model: $(basename $PIPER_MODEL)"

# -----------------------------------------------------------------------------
# Step 4: Build Firmware
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 4/7] Building Firmware...${NC}"
cd "$FIRMWARE_DIR"
make clean > /dev/null 2>&1 || true
make 2>&1 | while read line; do echo "  $line"; done

if [ ! -f "firmware.elf" ]; then
    echo -e "${RED}  ERROR: Firmware build failed${NC}"
    exit 1
fi
echo -e "  ${GREEN}✓${NC} firmware.elf built"

# -----------------------------------------------------------------------------
# Step 5: Build Software2
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 5/7] Building Software2...${NC}"
cd "$SOFTWARE2_DIR"
make clean > /dev/null 2>&1 || true
make 2>&1 | while read line; do echo "  $line"; done

if [ ! -f "bin/hampod" ]; then
    echo -e "${RED}  ERROR: Software2 build failed${NC}"
    exit 1
fi
echo -e "  ${GREEN}✓${NC} hampod built"

# -----------------------------------------------------------------------------
# Step 6: Start Firmware
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 6/7] Starting Firmware...${NC}"
cd "$FIRMWARE_DIR"
sudo ./firmware.elf > /tmp/firmware.log 2>&1 &
FIRMWARE_PID=$!
echo "  Firmware PID: $FIRMWARE_PID"

# Wait for Firmware_o pipe to exist
echo "  Waiting for Firmware to create pipes..."
for i in $(seq 1 30); do
    if [ -p "Firmware_o" ]; then
        break
    fi
    sleep 0.2
done

if [ ! -p "Firmware_o" ]; then
    echo -e "${RED}  ERROR: Firmware_o pipe not created${NC}"
    echo "  Check /tmp/firmware.log for errors"
    sudo kill $FIRMWARE_PID 2>/dev/null || true
    exit 1
fi
echo -e "  ${GREEN}✓${NC} Firmware started"

# -----------------------------------------------------------------------------
# Step 7: Run Software2 (hampod) for manual testing
# -----------------------------------------------------------------------------
echo -e "${YELLOW}[Step 7/7] Starting Software2 for manual testing...${NC}"
echo ""
echo -e "${CYAN}======================================================${NC}"
echo -e "${CYAN}              MANUAL TESTING MODE                     ${NC}"
echo -e "${CYAN}======================================================${NC}"
echo ""
echo -e "${GREEN}Normal Mode Keys:${NC}"
echo "  [2] - Announce current frequency"
echo "  [1] - Select VFO A, announce frequency"
echo "  [1] Hold - Select VFO B, announce frequency"
echo "  [0] - Announce current mode (USB, LSB, etc.)"
echo "  [*] - Announce S-meter reading"
echo "  [*] Hold - Announce power level"
echo "  [C] - Toggle auto-announcements on/off"
echo ""
echo -e "${GREEN}Frequency Mode Keys:${NC}"
echo "  [#] - Enter frequency mode, cycle VFO"
echo "  Digits - Enter frequency"
echo "  [#] - Submit frequency"
echo ""
echo -e "${YELLOW}Press Ctrl+C to exit when done${NC}"
echo ""
echo -e "${CYAN}======================================================${NC}"
echo ""

cd "$SOFTWARE2_DIR"

# Trap to cleanup on exit
cleanup() {
    echo ""
    echo -e "${YELLOW}Cleaning up...${NC}"
    sudo kill $FIRMWARE_PID 2>/dev/null || true
    sudo killall -9 firmware.elf piper aplay 2>/dev/null || true
    echo "Done."
}
trap cleanup EXIT

# Run hampod interactively
sudo ./bin/hampod 2>&1 | tee /tmp/hampod_output.txt

echo ""
echo -e "${CYAN}======================================================${NC}"
echo -e "${CYAN}                  TEST COMPLETE                       ${NC}"
echo -e "${CYAN}======================================================${NC}"
echo ""
echo "Please answer the following questions:"
echo ""
echo "  1. Did [2] announce the current frequency? (Y/N)"
echo "  2. Did [1] select VFO A and announce frequency? (Y/N)"
echo "  3. Did [1] Hold select VFO B? (Y/N)"
echo "  4. Did [0] announce the mode (USB, LSB, etc.)? (Y/N)"
echo "  5. Did [*] announce S-meter reading? (Y/N)"
echo "  6. Did [C] toggle announcements? (Y/N)"
echo ""
echo "If all answers are YES, the test PASSED."
echo "If any answer is NO, investigate the issue."
echo ""
echo "Logs preserved in /tmp/firmware.log and /tmp/hampod_output.txt"

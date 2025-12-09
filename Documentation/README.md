# HAMPOD Documentation

This directory contains documentation for the HAMPOD project.

## Contents
- [Project Overview](../README.md)
- [Raspberry Pi Migration Plan](Hampod%20RPi%20change%20plan.md)

## Migration Status

**Current Platform:** Raspberry Pi (migrated from NanoPi)

### Completed
- ✅ **Phase 1-3**: Hardware Abstraction Layer (HAL) implementation and integration
- ✅ USB Keypad support via HAL
- ✅ USB Audio support via HAL  
- ✅ WiringPi dependencies removed
- ✅ Firmware builds successfully on Raspberry Pi

### In Progress
- 🚧 Full system integration testing with Software layer

See [Hampod_RPi_change_plan.md](Hampod_RPi_change_plan.md) for detailed migration progress.

## Deployment

To deploy changes to the Raspberry Pi, we use a Git-based workflow (Push/Pull).

### Prerequisites

1.  **Git** must be installed on the Raspberry Pi (already verified).
2.  The repository must be cloned on the Raspberry Pi.

#### First-time Setup on Pi

SSH into the Pi and clone the repository:
```bash
ssh waynesr@HAMPOD.local
cd ~
# If the directory exists but is empty/not a repo, remove it first: rm -rf HAMPOD2026
git clone https://github.com/waynepadgett/HAMPOD2026.git
```

### Deployment Workflow

#### Step 1: Commit and Push from Local Machine

On your local machine (Windows), commit and push your changes to GitHub:
```bash
git add .
git commit -m "Your commit message"
git push origin main
```

#### Step 2: Sync to Raspberry Pi

You have two options to pull the latest changes on the Pi:

**Option A: One-liner (Recommended)**
```bash
ssh waynesr@HAMPOD.local "cd ~/HAMPOD2026 && git pull origin main"
```

**Option B: Interactive SSH**
```bash
ssh waynesr@HAMPOD.local
cd ~/HAMPOD2026
git pull origin main
exit
```

Both options achieve the same result. Option A is faster for quick syncs.


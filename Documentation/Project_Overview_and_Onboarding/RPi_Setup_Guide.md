# Raspberry Pi Development Setup Guide

This guide provides detailed, step-by-step instructions for setting up a Raspberry Pi (RPi) from scratch to develop and run the HAMPOD project.

## 📋 Prerequisites

Before you begin, ensure you have the following:

- **Raspberry Pi**: Recommended RPi 5 or RPi 4 (4GB+ RAM preferred, though 2GB works).
- **microSD Card**: 16GB or larger (Class 10 / UHS-1 recommended).
- **Power Supply**: Official USB-C power supply for your specific RPi model.
- **Computer**: A Windows, Mac, or Linux computer to flash the SD card.
- **Internet Connection**: Wi-Fi credentials or an Ethernet cable.

---

## 🚀 Step 1: Install Debian Trixie

The project is currently being developed on **Debian Trixie**. Please install this version using the Raspberry Pi Imager.

1.  **Download & Install Raspberry Pi Imager**:
    - Go to [raspberrypi.com/software](https://www.raspberrypi.com/software/) and download the imager for your computer.

2.  **Configure the Image**:
    - Open Raspberry Pi Imager.
    - **Choose Device**: Select your RPi model (e.g., Raspberry Pi 5).
    - **Choose OS**:
        - Click "Choose OS".
        - Select **General Purpose OS** -> **Debian** -> **Debian Trixie** (or the specific Trixie image if listed differently).
    - **Choose Storage**: Select your microSD card.

3.  **Advanced Options (Crucial Step)**:
    - Click "Next". When asked "Would you like to apply OS customisation settings?", verify **EDIT SETTINGS**.
    - **General Tab**:
        - **Set hostname**: `HAMPOD` (or whatever you prefer, this guide assumes `HAMPOD.local`).
        - **Set username and password**:
            - Username: `hampoduser`
            - Password: Choose a secure password.
        - **Configure Wireless LAN**: Enter your Wi-Fi SSID and password (if not using Ethernet).
    - **Services Tab**:
        - **Enable SSH**: Check "Use password authentication".
    - Click **SAVE** and then **YES** to apply.

4.  **Write and Verify**:
    - Click **YES** to begin writing. This will erase the SD card.
    - Once finished, insert the SD card into your Raspberry Pi and power it on.

---

## 🔌 Step 2: Connect via SSH

Give your Pi a minute or two to boot up and connect to the network.

1.  Open a terminal (Command Prompt or PowerShell on Windows, Terminal on Mac/Linux).
2.  Connect using the hostname and username you set:
    ```bash
    ssh hampoduser@HAMPOD.local
    ```
    *(If prompted about authenticity, type `yes`)*
3.  Enter your password when prompted.

> **Troubleshooting**: If `HAMPOD.local` doesn't work, check your router's device list to find the IP address of the Pi and use `ssh hampoduser@192.168.x.x` instead.

### Optional: Set up an SSH Key
To avoid typing your password every time, you can generate an SSH key on your computer and copy it to the Pi.

1.  **Generate a key** (if you don't have one):
    *On Windows (PowerShell) or Mac/Linux (Terminal):*
    ```bash
    ssh-keygen -t ed25519
    ```
    *(Press Enter to accept defaults. If using Windows Command Prompt, ensure OpenSSH is installed/enabled)*

2.  **Copy the key to the Pi**:
    *On Windows (PowerShell):*
    ```powershell
    type $env:USERPROFILE\.ssh\id_ed25519.pub | ssh hampoduser@HAMPOD.local "mkdir -p ~/.ssh && cat >> ~/.ssh/authorized_keys"
    ```
    *On Mac/Linux:*
    ```bash
    ssh-copy-id hampoduser@HAMPOD.local
    ```

3.  **Verify**: Log out and log back in. You should not be asked for a password.

---

## 🛠️ Step 3: Install System Dependencies

Once logged in, update the system and install the required build tools and libraries.

1.  **Update Package Lists**:
    ```bash
    sudo apt update && sudo apt upgrade -y
    ```
2.  **Install Development Tools & Audio Libraries**:
    HAMPOD requires `git`, `make`, `gcc`, ALSA (audio), and Hamlib (rig control).
    ```bash
    sudo apt install -y git make gcc libasound2-dev libhamlib-dev
    ```

---

## 📂 Step 4: Clone the HAMPOD Repository

Now, download the project code to your home directory.

1.  **Clone the Repository**:
    ```bash
    cd ~
    git clone https://github.com/waynepadgett/HAMPOD2026.git
    ```
2.  **Enter the Directory**:
    ```bash
    cd HAMPOD2026
    ```

---

## 🗣️ Step 5: Install Piper TTS

HAMPOD uses **Piper** for high-quality, low-latency text-to-speech. We have a script to automate this installation.

1.  **Make Scripts Executable**:
    ```bash
    chmod +x Documentation/scripts/*.sh
    ```
2.  **Run the Installer**:
    ```bash
    ./Documentation/scripts/install_piper.sh
    ```
    *This script downloads the Piper binary, installs it to `/usr/local/bin/piper`, and downloads the required voice model (`en_US-lessac-low.onnx`) to `Firmware/models/`.*

---

## 🏗️ Step 6: Build the Project

You need to build both the **Firmware** (which handles hardware/audio) and the **Software** (high-level logic).

### 1. Build Firmware
```bash
cd ~/HAMPOD2026/Firmware
make
```
*If successful, you will see a `firmware.elf` file created.*

### 2. Build Software
```bash
cd ~/HAMPOD2026/Software
make
```
*If successful, you will see a `Software.elf` file created.*

---

## ▶️ Step 7: Running HAMPOD

To run the full system, you typically need to run the Firmware and Software in separate terminals (or background screens), as they communicate via named pipes.

1.  **Start Firmware**:
    ```bash
    cd ~/HAMPOD2026/Firmware
    sudo ./firmware.elf
    ```
    *Note: `sudo` might be required for hardware access (GPIO/Audio) depending on your user groups, though standard users in the `audio` and `gpio` groups often work.*

2.  **Start Software** (in a new SSH session):
    ```bash
    cd ~/HAMPOD2026/Software
    ./Software.elf
    ```

### Optional: Running a Demonstration (Integration Test)

If the full Software layer is not yet ready, you can run the **HAL Integration Test**. This is a standalone test that verifies the keypad and audio subsystems are working together *without* needing the main Firmware/Software loop.

1.  **Build and Run the Test**:
    ```bash
    cd ~/HAMPOD2026/Firmware/hal/tests
    make
    sudo ./test_hal_integration
    ```

2.  **What to Expect**:
    - The system should announce "System Ready" (or similar).
    - Pressing keys on the keypad should announce the key name (e.g., "One", "Enter").
    - Use `Ctrl+C` to exit.
    - *This confirms that your hardware, audio drivers, and Piper TTS are all correctly configured.*

---

## 📝 Tips for Developers

- **Remote Install Script**: For rapid development, use the `remote_install.sh` script from your local machine to push code and rebuild on the Pi automatically.
  ```bash
  # From your local machine
  ./Documentation/scripts/remote_install.sh "Update description"
  ```

- **Cleaning Builds**: If you run into weird issues, try cleaning:
  ```bash
  make clean
  make
  ```

- **Process Cleanup (After a crash)**:
  If the firmware or software crashes, it might leave "ghost" processes or named pipes that prevent restarting. Run this to clean up:
  ```bash
  # Kill lingering processes
  sudo pkill -9 firmware
  sudo pkill -9 imitation_software
  pkill -9 Software.elf

  # Remove stale pipes
  rm -f ~/HAMPOD2026/Firmware/Firmware_i
  rm -f ~/HAMPOD2026/Firmware/Firmware_o
  ```

- **Remote Reboot**:
  If the system becomes unresponsive or audio drivers lock up:
  ```bash
  sudo reboot
  ```

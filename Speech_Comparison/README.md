# Speech Synthesizer Latency Comparison

This folder contains a standalone integration test to compare the latency of **Festival** (legacy) vs **Piper** (new) on the target hardware (Raspberry Pi).

## Overview

The `speech_latency_test` utility wraps the existing Firmware HAL (Keypad + Audio) and injects a speech synthesis step. It measures the time taken to generate the speech audio file.

## Prerequisites

1. **Festival** must be installed:
   ```bash
   sudo apt-get install festival festvox-kallpc16k
   ```

2. **Piper** must be installed and a voice model downloaded.
   
   **Install Piper (Raspberry Pi 64-bit / aarch64):**
   ```bash
   # Download Piper
   wget https://github.com/rhasspy/piper/releases/download/2023.11.14-2/piper_linux_aarch64.tar.gz
   tar -xvf piper_linux_aarch64.tar.gz
   
   # Add to PATH (optional, or just run from ./piper/piper)
   sudo cp -r piper/* /usr/local/bin/
   ```

   **Download Voice Models:**
   ```bash
   # Medium Quality (Recommended balance)
   wget -O en_US-lessac-medium.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx
   wget -O en_US-lessac-medium.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/medium/en_US-lessac-medium.onnx.json

   # Low Quality (Faster, lower quality)
   wget -O en_US-lessac-low.onnx https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/low/en_US-lessac-low.onnx
   wget -O en_US-lessac-low.onnx.json https://huggingface.co/rhasspy/piper-voices/resolve/main/en/en_US/lessac/low/en_US-lessac-low.onnx.json
   ```


   - Ensure `PIPER_MODEL_PATH` in `speech_latency_test.c` matches the location of `en_US-lessac-medium.onnx`.


## Compilation

To compile the test tool:

```bash
make
```

### Speed Control
You can optionally control the speech rate for Piper modes by adding a second argument (default is 1.0). Lower numbers are faster.

```bash
# Play at 1.25x speed (Legacy scale: 0.8)
sudo ./speech_latency_test piper-persistent 0.8

# Play at 1.5x speed (Legacy scale: 0.66)
sudo ./speech_latency_test piper-persistent 0.66
```

### How it Works

1. The program initializes the Keypad and Audio HALs.
2. It waits for a key press on the keypad.
3. Upon key press, it:
   - Captures start time.
   - Executes the synthesis command.
   - Captures end time.
   - Prints the **Latency** (in milliseconds).
   - Plays the audio (via file or stream).

## Learnings & Recommendations

Through testing on the Raspberry Pi 5, we have established the following hierarchy of performance:

| Strategy | Est. Latency | Quality | Notes |
| :--- | :--- | :--- | :--- |
| **Piper Persistent** | **< 1ms** | Good | **Recommended Architecture.** Keeps Piper running in background. Text is piped in, audio piped out. No startup cost. |
| **Piper Streaming** | ~50ms | Good | Pipes `piper --output_raw` directly to `aplay`. Avoids disk write, but still incurs ~300ms startup cost per word if not persistent. |
| **Piper Low (File)** | ~400ms+ | Acceptable | Uses optimized `lessac-low` model. Faster inference than Medium, but file I/O dominates latency. |
| **Piper Med (File)** | ~500ms+ | Best | High quality model, but slowest due to model size and file I/O. |
| **Festival** | ~150-300ms | Poor | Legacy engine. Fast, but robotic voice quality. |

### Key Optimization Findings
1.  **Architecture Matters Most**: The biggest latency bottleneck is **process startup** (loading the model). Using a persistent pipeline eliminates this entirely.
2.  **Streaming**: Piping audio (`| aplay`) is faster than writing to disk (`--output_file`), as playback starts immediately.
3.  **Sample Rate Mismatch**: The "Low Quality" model uses 16kHz audio, while "Medium" uses 22kHz. Playing 16kHz audio at 22kHz results in high-pitched "Chipmunk" speech. We fixed this by explicitly setting `aplay -r 16000`.
4.  **Speed Control**: Using `--length_scale` (e.g., 0.75) effectively speeds up speech without degrading quality significantly.

**Final Recommendation**: Use **Piper Persistent** mode with the **Low Quality** model (16kHz) for the best balance of zero-latency response and acceptable voice quality on the HAMPOD.


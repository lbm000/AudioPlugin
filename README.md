# Audiovisual Plugin

## Project Description

The Audiovisual Plugin is a JUCE-based audio processor that allows real-time manipulation of audio samples through a variety of effects. Each sample can be processed individually using filters such as low-pass, high-pass, band-pass, notch, and peak. The plugin also includes gain control, bitcrusher, and an ADSR envelope generator. It supports both VST3 and standalone formats, making it ideal for audio experimentation and creative sound design.

## Features

- Per-sample filter control:
    - Low-pass, high-pass, band-pass, notch, and peak filters
- Independent gain control for each sample
- Bitcrusher effect with customizable bit depth and downsampling rate
- ADSR (Attack, Decay, Sustain, Release) envelope shaping
- Global BPM synchronization
- Built with JUCE
- Supports VST3 and Standalone formats

## Installation

**Clone the repository**
   ```bash
   git clone https://github.com/lbm000/AudioPlugin.git
   cd AudioPlugin
   ```
   

If you already cloned the repo without --recurse-submodules, you can fix it with:

   ```bash
   git submodule update --init --recursive
   ```


**Install dependencies (Linux)**


```bash
sudo apt update
sudo apt install libcurl4-openssl-dev libgtk-3-dev libwebkit2gtk-4.1-dev
```

## Build Instructions

### Option 1: Using CLion

1. Open the project folder (`AudioPlugin`) in CLion.
2. Let CLion automatically configure the CMake profile.
3. Select the target `Audiovisual_Plugin_Standalone`.
4. Click the ▶️ Run or Build button to compile and execute.

### Option 2: Using CMake in Terminal (with Ninja)

```bash
# From the root directory of the project
cmake -S . -B build -G Ninja
cmake --build build
# Technical Documentation – Audiovisual Plugin

## 1. Architecture Overview

The project is structured into the following main modules:

- **Source/**
    - `PluginProcessor.*`: Handles audio processing logic.
    - `PluginEditor.*`: Manages the GUI of the plugin.
- **extern/JUCE**: External JUCE framework used for audio and GUI components.
- **CMakeLists.txt**: Project configuration and build instructions.

### Class Structure

- `SampleAudioProcessor`: Inherits from `juce::AudioProcessor`, responsible for DSP, state handling, and parameter management.
- `SampleAudioProcessorEditor`: Inherits from `juce::AudioProcessorEditor`, responsible for rendering and managing the GUI.

---

## 2. Signal Processing (DSP Components)

Each audio sample can be individually processed using the following components:

- **Filters**
    - Low-pass, High-pass, Band-pass, Notch, Peak 
- **Bitcrusher**
    - Adjustable bit depth and downsampling
- **Gain**
    - Individual gain control per sample
- **ADSR Envelope**
    - Attack, Decay, Sustain, Release using `juce::ADSR`
- **Global BPM Sync**
    - Processing can be timed based on host BPM

---

## 3. GUI Layout

The plugin's GUI includes:

- Toggle buttons for each filter type per sample
- Rotary sliders for frequency, Q, gain, and other parameters
- Interactive ADSR curve
- Grouped layout per sample using `juce::GroupComponent`
- Optional real-time waveform display (if implemented)

---

## 4. Signal Flow

1. Sample is loaded
2. Upon playback:
    - ADSR envelope is applied independently, based on user selection.
    - Filters are applied independently, based on user selection.
    - Bitcrusher is applied, based on user selection.
    - Gain adjustment, based on user selection.
3. Output is passed to the main audio bus

---

## 5. Special Features

- **Modular DSP structure**: Each effect is implemented independently per sample
- **Mutual exclusion logic**: Some filters disable others for clarity (e.g., Peak vs Band-pass)
- **ADSR activation via `noteOn()`/`noteOff()`** logic synchronized with sample playback
- **Optional custom components**: GUI styled with custom rotary knobs, intuitive grouping, and custom labels

---

## 6. External Libraries

- **JUCE** (included via `extern/JUCE`)
    - Audio processing
    - Plugin wrapper (VST3/Standalone)
    - GUI components
- **GTK3, WebKit2, libcurl** (Linux only)
    - Required for JUCE standalone compatibility
    - Installed via `apt`:
      ```bash
      sudo apt install libcurl4-openssl-dev libgtk-3-dev libwebkit2gtk-4.1-dev
      ```

---



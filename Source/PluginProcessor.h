#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>


/**
 * @class SampleAudioProcessor
 * @brief Audio plugin processor that manages sample playback, effects, filters, and step sequencing.
 *
 * This class handles all audio logic, including loading samples, applying effects like low-pass, high-pass,
 * band-pass, notch, and peak filters, bitcrusher, gain control, ADSR envelopes, and BPM-synchronized step sequencing.
 */

class SampleAudioProcessor  : public juce::AudioProcessor
{
public:

    /** Constructor */
    SampleAudioProcessor();

    /** Destructor */
    ~SampleAudioProcessor() override;


    /**
     * @brief Prepares the processor to start playback.
     * @param sampleRate The current sample rate.
     * @param samplesPerBlock Expected buffer size.
     */
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;

    /**
 * @brief Releases any resources used by the processor.
 */
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations

    /**
     * @brief Checks whether a given channel layout is supported.
     * @param layouts The input/output layout configuration.
     * @return true if supported, false otherwise.
     */
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif


    /**
     * @brief Main audio processing method.
     * @param buffer Audio buffer to process.
     * @param midiMessages MIDI buffer.
     */
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;


    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    /**
     * @brief Loads an audio sample from a file into a given slot.
     * @param file Audio file to load.
     * @param index Slot index to load into (0 to NUM_SAMPLES-1).
     */
    void loadSampleFile(const juce::File& file, int index);

    /**
     * @brief Sets the global BPM value.
     * @param newBpm The new BPM to use.
     */
    void setGlobalBpm(float newBpm);

    /**
     * @brief Returns the current global BPM value.
     * @return Current BPM.
     */
    float getGlobalBpm() const { return globalBpm; }

    /** @brief Total number of supported samples. */
    static constexpr int NUM_SAMPLES = 5;


    /** @brief Tracks whether each sample is currently playing. */
    std::array<bool, NUM_SAMPLES> isSamplePlaying {};

    /**
     * @brief Returns the current step in the sequencer.
     * @return Step index (0 to NUM_STEPS-1).
     */
    int getCurrentStep() const { return currentStep; }

    /**
     * @brief Enables or disables a step in the sequencer.
     * @param track Track index.
     * @param step Step index.
     * @param isOn Whether the step should be active.
     */
    void setStepState(int track, int step, bool isOn)
    {
        stepStates[track][step] = isOn;
    }

    /** @brief Enables or disables the low-pass filter for a given sample. */
    void setFilterEnabled(int index, bool enabled);

    /** @brief Sets the low-pass filter cutoff frequency for a given sample. */
    void setFilterCutoff(int index, float cutoffHz);

    /** @brief Checks whether the low-pass filter is enabled. */
    bool getFilterEnabled(int index) const { return isFilterEnabled[index]; }

    /** @brief Checks whether the high-pass filter is enabled. */
    bool getHighpassEnabled(int index) const;

    /** @brief Enables or disables the high-pass filter for a sample. */
    void setHighpassEnabled(int index, bool enabled);

    /** @brief Returns the high-pass cutoff frequency. */
    float getHighpassCutoff(int index) const;

    /** @brief Sets the high-pass cutoff frequency. */
    void setHighpassCutoff(int index, float cutoff);

    /** @brief Checks whether the band-pass filter is enabled. */
    bool getBandPassEnabled(int index) const;

    /** @brief Enables or disables the band-pass filter. */
    void setBandPassEnabled(int index, bool enabled);

    /** @brief Sets the band-pass cutoff frequency. */
    void setBandPassCutoff(int index, float value);

    /** @brief Gets the band-pass cutoff frequency. */
    float getBandPassCutoff(int index) const;

    /** @brief Sets the band-pass bandwidth (Q factor). */
    void setBandPassBandwidth(int index, float value);

    /** @brief Gets the band-pass bandwidth (Q factor). */
    float getBandPassBandwidth(int index) const;

    /** @brief Enables or disables the notch filter. */
    void setNotchEnabled(int index, bool enabled);

    /** @brief Checks whether the notch filter is enabled. */
    bool getNotchEnabled(int index) const;

    /** @brief Sets the notch filter cutoff frequency. */
    void setNotchCutoff(int index, float value);

    /** @brief Sets the notch filter bandwidth. */
    void setNotchBandwidth(int index, float value);

    /** @brief Checks whether the peak filter is enabled. */
    bool getPeakEnabled(int index) const;

    /** @brief Enables or disables the peak (bell) filter. */
    void setPeakEnabled(int index, bool enabled);

    /** @brief Sets the peak filter cutoff frequency. */
    void setPeakCutoff(int index, float value);

    /** @brief Sets the gain of the peak filter. */
    void setPeakGain(int index, float value);

    /** @brief Sets the Q factor (width) of the peak filter. */
    void setPeakQ(int index, float value);

    /** @brief Enables or disables the bitcrusher effect. */
    void setBitcrusherEnabled(int index, bool enabled);

    /** @brief Sets the bit depth for the bitcrusher. */
    void setBitDepth(int index, int depth);

    /** @brief Sets the downsampling rate for the bitcrusher. */
    void setDownsampleRate(int index, float rate);

    /** @brief Sets the gain level for a sample. */
    void setGainLevel(int index, float gain);

    /**  @brief Gets the gain level for a sample. */
    float getGainLevel(int index) const;

    /** @brief Sets the ADSR attack value for a sample. */
    void setAdsrAttack(int index, float value);

    /** @brief Sets the ADSR decay value for a sample. */
    void setAdsrDecay(int index, float value);

    /** @brief Sets the ADSR sustain level for a sample. */
    void setAdsrSustain(int index, float value);

    /** @brief Sets the ADSR release value for a sample. */
    void setAdsrRelease(int index, float value);

    /** @brief Gets the ADSR attack value. */
    float getAdsrAttack(int index) const;

    /** @brief Gets the ADSR decay value. */
    float getAdsrDecay(int index) const;

    /** @brief Gets the ADSR sustain level. */
    float getAdsrSustain(int index) const;

    /** @brief Gets the ADSR release value. */
    float getAdsrRelease(int index) const;




private:
    /* @brief Manages audio format readers and writers. */
    juce::AudioFormatManager formatManager;

    /* @brief Audio buffers for each loaded sample. */
    std::array<juce::AudioBuffer<float>, NUM_SAMPLES> sampleBuffers;

    /* @brief  Current read positions for each sample buffer. */
    std::array<int, NUM_SAMPLES> sampleReadPositions {};

    /* @brief Indicates whether a sample file has been successfully loaded. */
    std::array<bool, NUM_SAMPLES> isSampleFileLoaded {};

    /* @brief  Sample playback counters, useful for synchronization. */
    std::array<int, NUM_SAMPLES> SampleCounters {};

    /* @brief Global BPM used for timing and sequencing.*/
    float globalBpm = 120.0f;

    /* @brief Number of samples per beat, calculated from BPM and sample rate.*/
    int globalSamplesPerBeat = 0;

    /* @brief Total number of steps in the step sequencer */
    static constexpr int NUM_STEPS = 16;

    /* @brief Total number of sample slots .*/
    static constexpr int NUM_TRACKS = 6;

    /* @brief Sequencer state: true if a step is active, false otherwise. Accessed as [track][step].  */
    std::array<std::array<bool, NUM_STEPS>, NUM_TRACKS> stepStates {};

    /* @brief Index of the current step being played. */
    int currentStep = 0;

    /* @brief Internal sample counter used to trigger step advancement.*/
    int sampleCounterForStep = 0;


    //================== Low-Pass Filter ==================

    /**
     * @brief State-variable low-pass filters applied independently to each sample slot.
     *
     * These filters remove high-frequency content from each sample using JUCE's
     * StateVariableTPTFilter in low-pass mode.
     */
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_SAMPLES> sampleFilters;

    /**
     * @brief Flags indicating whether the low-pass filter is enabled for each sample.
     */
    std::array<bool, NUM_SAMPLES> isFilterEnabled {};

    /**
     * @brief Cutoff frequencies (in Hz) for each sample's low-pass filter.
     */
    std::array<float, NUM_SAMPLES> cutoffFrequencies {};


    //================== High-Pass Filter ==================

    /**
     * @brief High-pass filters for each sample using state-variable TPT filters.
     *
     * These filters remove low-frequency content from each sample.
     */
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_SAMPLES> sampleHighPassFilters;

    /**
     * @brief Flags indicating whether the high-pass filter is enabled for each sample.
     */
    std::array<bool, NUM_SAMPLES> isHighPassEnabled {};

    /**
     * @brief Cutoff frequencies (in Hz) for each sample's high-pass filter.
     */
    std::array<float, NUM_SAMPLES> highPassCutoffFrequencies {};


    //================== Band-Pass Filter ==================

    /**
     * @brief Band-pass filters for each sample using state-variable TPT filters.
     *
     * These filters isolate a specific frequency band from the input signal.
     */
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_SAMPLES> sampleBandPassFilters;

    /**
     * @brief Flags indicating whether the band-pass filter is enabled for each sample.
     */
    std::array<bool, NUM_SAMPLES> isBandPassEnabled {};

    /**
     * @brief Center cutoff frequencies (in Hz) for the band-pass filters.
     */
    std::array<float, NUM_SAMPLES> bandPassCutoffs;

    /**
     * @brief Bandwidths (Q factors) for the band-pass filters.
     */
    std::array<float, NUM_SAMPLES> bandPassBandwidths;


    //================== Notch Filter ==================

    /**
     * @brief Notch filters for each sample using IIR filters.
     *
     * These filters attenuate a narrow frequency band to suppress unwanted frequencies.
     */
    std::array<juce::dsp::IIR::Filter<float>, NUM_SAMPLES> sampleNotchFilters;

    /**
     * @brief Flags indicating whether the notch filter is enabled for each sample.
     */
    std::array<bool, NUM_SAMPLES> isNotchEnabled {};

    /**
     * @brief Cutoff frequencies (in Hz) for the notch filters.
     */
    std::array<float, NUM_SAMPLES> notchCutoffs {};

    /**
     * @brief Bandwidths for the notch filters.
     */
    std::array<float, NUM_SAMPLES> notchBandwidths {};

    /**
     * @brief Updates the internal filter coefficients for a notch filter.
     * @param index The index of the sample whose notch filter should be updated.
     */
    void updateNotchCoefficients(int index);


    //================== Peak Filter ==================

    /**
     * @brief Peak (bell-shaped) filters for each sample using IIR filters.
     *
     * These filters allow for frequency-specific boosting or attenuation.
     */
    std::array<juce::dsp::IIR::Filter<float>, NUM_SAMPLES> samplePeakFilters;

    /**
     * @brief Flags indicating whether the peak filter is enabled for each sample.
     */
    std::array<bool, NUM_SAMPLES> isPeakEnabled;

    /**
     * @brief Center frequencies (in Hz) for the peak filters.
     */
    std::array<float, NUM_SAMPLES> peakCutoffs;

    /**
     * @brief Gain values (in dB) for the peak filters.
     */
    std::array<float, NUM_SAMPLES> peakGains;

    /**
     * @brief Q values (bandwidth) for the peak filters.
     */
    std::array<float, NUM_SAMPLES> peakQs;


    //================== Bitcrusher ==================

    /**
     * @brief Flags indicating whether the bitcrusher effect is enabled per sample.
     */
    std::array<bool, NUM_SAMPLES> isBitcrusherEnabled {};

    /**
     * @brief Bit depths used for reducing resolution in the bitcrusher effect.
     */
    std::array<int, NUM_SAMPLES> bitDepths {};

    /**
     * @brief Downsampling rates for the bitcrusher effect.
     */
    std::array<float, NUM_SAMPLES> downsampleRates {};

    /**
     * @brief Internal counters used to track downsampling intervals.
     */
    std::array<int, NUM_SAMPLES> downsampleCounters {};


    //================== Gain ==================

    /**
     * @brief Gain levels (linear scale) applied to each sample.
     */
    std::array<float, NUM_SAMPLES> gainLevels { 1.0f };


    //================== ADSR ==================

    /**
     * @brief ADSR parameter sets for each sample (attack, decay, sustain, release).
     */
    juce::ADSR::Parameters adsrParams[NUM_SAMPLES];

    /**
     * @brief ADSR envelope processors for each sample.
     */
    juce::ADSR adsrEnvelopes[NUM_SAMPLES];


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleAudioProcessor)

};


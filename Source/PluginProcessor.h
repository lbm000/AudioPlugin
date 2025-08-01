#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class SampleAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    SampleAudioProcessor();
    ~SampleAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //===================================


    void loadSampleFile(const juce::File& file, int index);
    void loadBeatFile(const juce::File& file, int index);

    void setGlobalBpm(float newBpm);
    float getGlobalBpm() const { return globalBpm; }
    static constexpr int NUM_SAMPLES = 5;

    std::array<bool, NUM_SAMPLES> isSamplePlaying {};

    int getCurrentStep() const { return currentStep; }

    void setStepState(int track, int step, bool isOn)
    {
        stepStates[track][step] = isOn;
    }

    void setFilterEnabled(int index, bool enabled);


    void setFilterCutoff(int index, float cutoffHz);

    bool getFilterEnabled(int index) const { return isFilterEnabled[index]; }

    bool getHighpassEnabled(int index) const;
    void setHighpassEnabled(int index, bool enabled);

    float getHighpassCutoff(int index) const;
    void setHighpassCutoff(int index, float cutoff);

    bool getBandPassEnabled(int index) const;
    void setBandPassEnabled(int index, bool enabled);


    void setBandPassCutoff(int index, float value);
    float getBandPassCutoff(int index) const;

    void setBandPassBandwidth(int index, float value);
    float getBandPassBandwidth(int index) const;

    void setNotchEnabled(int index, bool enabled);
    bool getNotchEnabled(int index) const;
    void setNotchCutoff(int index, float value);
    void setNotchBandwidth(int index, float value);

    bool getPeakEnabled(int index) const;
    void setPeakEnabled(int index, bool enabled);

    void setPeakCutoff(int index, float value);
    void setPeakGain(int index, float value);
    void setPeakQ(int index, float value);

    void setBitcrusherEnabled(int index, bool enabled);
    void setBitDepth(int index, int depth);
    void setDownsampleRate(int index, float rate);

    void setGainLevel(int index, float gain);
    float getGainLevel(int index) const;

    void setAdsrAttack(int index, float value);
    void setAdsrDecay(int index, float value);
    void setAdsrSustain(int index, float value);
    void setAdsrRelease(int index, float value);

    float getAdsrAttack(int index) const;
    float getAdsrDecay(int index) const;
    float getAdsrSustain(int index) const;
    float getAdsrRelease(int index) const;




private:
    juce::AudioFormatManager formatManager;

    // Buffers and internal control
    std::array<juce::AudioBuffer<float>, NUM_SAMPLES> sampleBuffers;
    std::array<int, NUM_SAMPLES> sampleReadPositions {};
    std::array<bool, NUM_SAMPLES> isSampleFileLoaded {};
    std::array<int, NUM_SAMPLES> SampleCounters {};


    float globalBpm = 120.0f;
    int globalSamplesPerBeat = 0;

    static constexpr int NUM_STEPS = 16;
    static constexpr int NUM_TRACKS = 6;
    std::array<std::array<bool, NUM_STEPS>, NUM_TRACKS> stepStates {}; // [track][step]
    int currentStep = 0;
    int sampleCounterForStep = 0;


    // filter low pass
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_SAMPLES> sampleFilters;
    std::array<bool, NUM_SAMPLES> isFilterEnabled {};
    std::array<float, NUM_SAMPLES> cutoffFrequencies {};

    // filter high pass
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_SAMPLES> sampleHighPassFilters;
    std::array<bool, NUM_SAMPLES> isHighPassEnabled {};
    std::array<float, NUM_SAMPLES> highPassCutoffFrequencies {};

    // filter band pass
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_SAMPLES> sampleBandPassFilters;
    std::array<bool, NUM_SAMPLES> isBandPassEnabled {};
    std::array<float, NUM_SAMPLES> bandPassCutoffs;
    std::array<float, NUM_SAMPLES> bandPassBandwidths;

    // Notch Filter
    std::array<juce::dsp::IIR::Filter<float>, NUM_SAMPLES> sampleNotchFilters;
    std::array<bool, NUM_SAMPLES> isNotchEnabled {};
    std::array<float, NUM_SAMPLES> notchCutoffs {};
    std::array<float, NUM_SAMPLES> notchBandwidths {};
    void updateNotchCoefficients(int index);

    // peak filter
    std::array<juce::dsp::IIR::Filter<float>, NUM_SAMPLES> samplePeakFilters;
    std::array<bool, NUM_SAMPLES> isPeakEnabled;
    std::array<float, NUM_SAMPLES> peakCutoffs;
    std::array<float, NUM_SAMPLES> peakGains;
    std::array<float, NUM_SAMPLES> peakQs;

    // Bitcrusher
    std::array<bool, NUM_SAMPLES> isBitcrusherEnabled {};
    std::array<int, NUM_SAMPLES> bitDepths {};
    std::array<float, NUM_SAMPLES> downsampleRates {};
    std::array<int, NUM_SAMPLES> downsampleCounters {};

    // gain
    std::array<float, NUM_SAMPLES> gainLevels { 1.0f };

    //adsr
    juce::ADSR::Parameters adsrParams[NUM_SAMPLES];
    juce::ADSR adsrEnvelopes[NUM_SAMPLES];








    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleAudioProcessor)
};


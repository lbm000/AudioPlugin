#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_dsp/juce_dsp.h>

//==============================================================================
class AnimalBeatAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    AnimalBeatAudioProcessor();
    ~AnimalBeatAudioProcessor() override;

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


    void loadAnimalFile(const juce::File& file, int index);
    void loadBeatFile(const juce::File& file, int index);

    void setGlobalBpm(float newBpm);
    float getGlobalBpm() const { return globalBpm; }
    static constexpr int NUM_ANIMALS = 4;
    static constexpr int NUM_BEATS = 2;

    std::array<bool, NUM_ANIMALS> isAnimalPlaying {};
    std::array<bool, NUM_BEATS> isBeatPlaying {};

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

    bool getBeatFilterEnabled(int index) const { return beatFilterEnabled[index]; }
    void setBeatFilterEnabled(int index, bool b) { beatFilterEnabled[index] = b; }

    float getBeatFilterCutoff(int index) const { return beatFilterCutoff[index]; }
    void setBeatFilterCutoff(int index, float f) { beatFilterCutoff[index] = f; }

    bool getBeatHighpassEnabled(int index) const { return beatHighpassEnabled[index]; }
    void setBeatHighpassEnabled(int index, bool b) { beatHighpassEnabled[index] = b; }

    float getBeatHighpassCutoff(int index) const { return beatHighpassCutoff[index]; }
    void setBeatHighpassCutoff(int index, float f) { beatHighpassCutoff[index] = f; }

    bool getBeatBandPassEnabled(int index) const { return beatBandPassEnabled[index]; }
    void setBeatBandPassEnabled(int index, bool b) { beatBandPassEnabled[index] = b; }

    float getBeatBandPassCutoff(int index) const { return beatBandPassCutoff[index]; }
    void setBeatBandPassCutoff(int index, float f) { beatBandPassCutoff[index] = f; }

    float getBeatBandPassBandwidth(int index) const { return beatBandPassBandwidth[index]; }
    void setBeatBandPassBandwidth(int index, float f) { beatBandPassBandwidth[index] = f; }



private:
    juce::AudioFormatManager formatManager;

    // Buffers and internal control
    std::array<juce::AudioBuffer<float>, NUM_ANIMALS> animalBuffers;
    std::array<int, NUM_ANIMALS> animalReadPositions {};
    std::array<bool, NUM_ANIMALS> isAnimalFileLoaded {};
    std::array<int, NUM_ANIMALS> animalSampleCounters {};

    std::array<juce::AudioBuffer<float>, NUM_BEATS> beatBuffers;
    std::array<int, NUM_BEATS> beatReadPositions {};
    std::array<bool, NUM_BEATS> isBeatFileLoaded {};
    std::array<int, NUM_BEATS> beatSampleCounters {};

    float globalBpm = 120.0f;
    int globalSamplesPerBeat = 0;

    static constexpr int NUM_STEPS = 16;
    static constexpr int NUM_TRACKS = 6;
    std::array<std::array<bool, NUM_STEPS>, NUM_TRACKS> stepStates {}; // [track][step]
    int currentStep = 0;
    int sampleCounterForStep = 0;


    // filter low pass
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_ANIMALS> animalFilters;
    std::array<bool, NUM_ANIMALS> isFilterEnabled {};
    std::array<float, NUM_ANIMALS> cutoffFrequencies {};

    // filter high pass
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_ANIMALS> animalHighPassFilters;
    std::array<bool, NUM_ANIMALS> isHighPassEnabled {};
    std::array<float, NUM_ANIMALS> highPassCutoffFrequencies {};

    // filter band pass
    std::array<juce::dsp::StateVariableTPTFilter<float>, NUM_ANIMALS> animalBandPassFilters;
    std::array<bool, NUM_ANIMALS> isBandPassEnabled {};
    std::array<float, NUM_ANIMALS> bandPassCutoffs;
    std::array<float, NUM_ANIMALS> bandPassBandwidths;

    std::array<bool, NUM_BEATS> beatFilterEnabled;
    std::array<float, NUM_BEATS> beatFilterCutoff;

    std::array<bool, NUM_BEATS> beatHighpassEnabled;
    std::array<float, NUM_BEATS> beatHighpassCutoff;

    std::array<bool, NUM_BEATS> beatBandPassEnabled;
    std::array<float, NUM_BEATS> beatBandPassCutoff;
    std::array<float, NUM_BEATS> beatBandPassBandwidth;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimalBeatAudioProcessor)
};


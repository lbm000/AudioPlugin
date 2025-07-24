#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>

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

    void setAnimalBpm(int index, float newBpm);
    void setBeatBpm(int index, float newBpm);
    void loadAnimalFile(const juce::File& file, int index);
    void loadBeatFile(const juce::File& file, int index);

    static constexpr int NUM_ANIMALS = 4;
    static constexpr int NUM_BEATS = 2;

    std::array<bool, NUM_ANIMALS> isAnimalPlaying {};
    std::array<bool, NUM_BEATS> isBeatPlaying {};
    std::array<float, NUM_ANIMALS> animalBpms {120.0f, 120.0f, 120.0f, 120.0f};
    std::array<float, NUM_BEATS> beatBpms {120.0f, 120.0f};

private:
    juce::AudioFormatManager formatManager;

    // Buffers and internal control
    std::array<juce::AudioBuffer<float>, NUM_ANIMALS> animalBuffers;
    std::array<int, NUM_ANIMALS> animalReadPositions {};
    std::array<bool, NUM_ANIMALS> isAnimalFileLoaded {};
    std::array<int, NUM_ANIMALS> animalSamplesPerBeat {};
    std::array<int, NUM_ANIMALS> animalSampleCounters {};

    std::array<juce::AudioBuffer<float>, NUM_BEATS> beatBuffers;
    std::array<int, NUM_BEATS> beatReadPositions {};
    std::array<bool, NUM_BEATS> isBeatFileLoaded {};
    std::array<int, NUM_BEATS> beatSamplesPerBeat {};
    std::array<int, NUM_BEATS> beatSampleCounters {};

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimalBeatAudioProcessor)
};

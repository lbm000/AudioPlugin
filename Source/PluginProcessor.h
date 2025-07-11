/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>
#include <juce_audio_formats/juce_audio_formats.h>

//==============================================================================
/**
*/
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

    void loadBeatFile(const juce::File& file);
    void loadAnimalFile(const juce::File& file);

    // flags for play and pause
    bool isAnimalPlaying = false;
    bool isBeatPlaying = false;

    // bpm standard value for start
    float animalBpm = 120.0f;
	float beatBpm = 120.0f;

    void setAnimalBpm(float newBpm);
    void setBeatBpm(float newBpm);


private:
    juce::AudioFormatManager formatManager;

    // buffer for animal sound
    juce::AudioBuffer<float> animalBuffer;

    // buffer for beats
	juce::AudioBuffer<float> beatBuffer;

    // currently position of buffers
    int animalReadPosition = 0;
	int drumReadPosition = 0;

    bool isAnimalFileLoaded = false;
    bool isBeatFileLoaded = false;

	int animalSamplesPerBeat = 0;
	int beatSamplesPerBeat = 0;

	int animalSampleCounter = 0;
	int beatSampleCounter = 0;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimalBeatAudioProcessor)
};

/*
==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>

#include "PluginProcessor.h"


//==============================================================================
/**
*/
class AnimalBeatAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    AnimalBeatAudioProcessorEditor (AnimalBeatAudioProcessor&);
    ~AnimalBeatAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AnimalBeatAudioProcessor& audioProcessor;

    static constexpr int NUM_ANIMALS = 4;
    static constexpr int NUM_BEATS = 2;

    std::array<juce::TextButton, NUM_ANIMALS> loadAnimalButtons;
    std::array<juce::ToggleButton, NUM_ANIMALS> playAnimalToggles;
    std::array<juce::Slider, NUM_ANIMALS> animalBpmSliders;

    std::array<juce::TextButton, NUM_BEATS> loadBeatButtons;
    std::array<juce::ToggleButton, NUM_BEATS> playBeatToggles;
    std::array<juce::Slider, NUM_BEATS> beatBpmSliders;

    std::array<juce::Label, NUM_ANIMALS + NUM_BEATS> bpmLabels;

    // Pointer for selection of audio files
    std::unique_ptr<juce::FileChooser> fileChooser;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimalBeatAudioProcessorEditor)
};

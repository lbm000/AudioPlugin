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
    juce::TextButton loadAnimalButton;
    juce::TextButton loadBeatButton;


    juce::ToggleButton playAnimalToggle { "Play Animal" };
    juce::ToggleButton playBeatToggle { "Play Beat" };

    /*
        slider to get bpm values, bpm value is like a scale to manage our "music"
        with this scale we can guarantee that a specifical sound will play at certain interval of time
     */
    juce::Slider animalBpmSlider;
    juce::Slider beatBpmSlider;

    //text for bpm slider
    juce::Label bpmLabel;

    // Pointer for selection of audio files
    std::unique_ptr<juce::FileChooser> fileChooser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimalBeatAudioProcessorEditor)
};

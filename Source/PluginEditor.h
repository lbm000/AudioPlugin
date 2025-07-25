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


static constexpr int NUM_TRACKS = AnimalBeatAudioProcessor::NUM_ANIMALS + AnimalBeatAudioProcessor::NUM_BEATS;
static constexpr int NUM_STEPS = 16;


class StepButton : public juce::Button
{
public:
    StepButton() : juce::Button("Step") {}

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();


        auto bgColour = getToggleState() ? juce::Colours::white : findColour(juce::ResizableWindow::backgroundColourId);
        auto borderColour = juce::Colours::white;

        g.setColour(bgColour);
        g.fillRect(bounds);

        g.setColour(borderColour);
        g.drawRect(bounds, 1.0f);
    }


};


//==============================================================================
/**
*/
class AnimalBeatAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    AnimalBeatAudioProcessorEditor (AnimalBeatAudioProcessor&);
    ~AnimalBeatAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AnimalBeatAudioProcessor& audioProcessor;

    static constexpr int NUM_ANIMALS = 4;
    static constexpr int NUM_BEATS = 2;

    std::array<juce::TextButton, NUM_ANIMALS> loadAnimalButtons;
    std::array<juce::TextButton, NUM_ANIMALS> playAnimalButtons;


    std::array<juce::TextButton, NUM_BEATS> loadBeatButtons;
    std::array<juce::TextButton, NUM_BEATS> playBeatButtons;

    juce::Slider globalBpmSlider;
    juce::Label globalBpmLabel;


    // Pointer for selection of audio files
    std::unique_ptr<juce::FileChooser> fileChooser;

    StepButton stepButtons[NUM_TRACKS][NUM_STEPS];

    std::array<juce::Label, NUM_STEPS> stepLabels;
    juce::GroupComponent stepSequencerGroup;

    int currentStep = 0;





    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnimalBeatAudioProcessorEditor)
};




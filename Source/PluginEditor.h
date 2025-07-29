#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>

#include "PluginProcessor.h"


static constexpr int NUM_TRACKS = SampleAudioProcessor::NUM_SAMPLES;
static constexpr int NUM_STEPS = 16;

class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    CustomLookAndFeel()
    {
        setColour(juce::TextButton::buttonOnColourId, juce::Colours::deepskyblue);
    }

    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                          const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto centreX = bounds.getCentreX();
        auto centreY = bounds.getCentreY();
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);

        auto knobBodyColour = juce::Colour(0xff333333);
        auto arcColour = slider.isMouseOverOrDragging() ? juce::Colours::deepskyblue.brighter() : juce::Colours::deepskyblue;
        auto pointerColour = juce::Colours::whitesmoke;
        auto shadowColour = juce::Colours::black.withAlpha(0.4f);

        auto shadowBounds = bounds.translated(1.0f, 2.0f);
        g.setColour(shadowColour);
        g.fillEllipse(shadowBounds);

        juce::ColourGradient gradient(knobBodyColour.brighter(0.1f), bounds.getTopLeft(), knobBodyColour.darker(0.1f), bounds.getBottomLeft(), false);
        g.setGradientFill(gradient);
        g.fillEllipse(bounds);
        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.drawEllipse(bounds, 1.5f);

        juce::Path valueArc;
        float arcThickness = 0.15f;
        valueArc.addPieSegment(bounds, rotaryStartAngle, angle, 1.0f - arcThickness);
        g.setColour(arcColour);
        g.fillPath(valueArc);

        juce::Path p;
        auto pointerLength = radius * 0.9f;
        auto pointerThickness = 3.0f;
        p.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        p.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
        g.setColour(pointerColour);
        g.fillPath(p);

        g.setColour(knobBodyColour.brighter(0.2f));
        g.fillEllipse(centreX - 4, centreY - 4, 8, 8);

        g.setColour(juce::Colours::grey);
        for (int i = 0; i < 11; ++i)
        {
            float proportion = i / 10.0f;
            float tickAngle = rotaryStartAngle + proportion * (rotaryEndAngle - rotaryStartAngle);
            juce::Path tick;
            tick.addRectangle(0.0f, -radius * 1.05f, 1.5f, radius * 0.1f);
            tick.applyTransform(juce::AffineTransform::rotation(tickAngle).translated(centreX, centreY));
            g.fillPath(tick);
        }
    }
};


class StepButton : public juce::Button
{
public:
    StepButton() : juce::Button("Step") {}

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto cornerSize = 4.0f;

        auto baseColour = getToggleState() ? juce::Colours::orange : juce::Colours::darkgrey.darker();
        if (shouldDrawButtonAsDown)
            baseColour = baseColour.darker();

        g.setColour(baseColour);
        g.fillRoundedRectangle(bounds, cornerSize);

        if (getToggleState())
        {
            g.setGradientFill(juce::ColourGradient(juce::Colours::white.withAlpha(0.3f), bounds.getCentre(), juce::Colours::white.withAlpha(0.0f), bounds.getBottomRight(), false));
            g.fillRoundedRectangle(bounds.reduced(1.0f), cornerSize);
        }

        g.setColour(juce::Colours::black.withAlpha(0.8f));
        g.drawRoundedRectangle(bounds, cornerSize, 1.5f);
    }


};


class StepHighlightOverlay : public juce::Component
{
public:
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
    }
};


//==============================================================================
/**
*/
class SampleAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:
    SampleAudioProcessorEditor (SampleAudioProcessor&);
    ~SampleAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;




private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SampleAudioProcessor& audioProcessor;

    static constexpr int NUM_SAMPLES = 5;
    static constexpr int NUM_BEATS = 2;

    std::array<juce::TextButton, NUM_SAMPLES> loadSampleButtons;
    std::array<juce::TextButton, NUM_SAMPLES> playSampleButtons;


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

    juce::TextButton filterToggleButtons[NUM_SAMPLES];
    juce::Slider filterCutoffSliders[NUM_SAMPLES];

    std::array<juce::TextButton, NUM_SAMPLES> highpassToggleButtons;
    std::array<juce::Slider, NUM_SAMPLES> highpassCutoffSliders;


    std::array<juce::TextButton, NUM_SAMPLES> bandpassToggleButtons;
    std::array<juce::Slider, NUM_SAMPLES> bandpassCutoffSliders;
    std::array<juce::Slider, NUM_SAMPLES> bandpassBandwidthSliders;


    std::array<juce::TextButton, NUM_SAMPLES> notchToggleButtons;
    std::array<juce::Slider, NUM_SAMPLES> notchCutoffSliders;
    std::array<juce::Slider, NUM_SAMPLES> notchBandwidthSliders;

    std::array<juce::TextButton, NUM_SAMPLES> peakToggleButtons;
    std::array<juce::Slider, NUM_SAMPLES> peakCutoffSliders;
    std::array<juce::Slider, NUM_SAMPLES> peakGainSliders;
    std::array<juce::Slider, NUM_SAMPLES> peakQSliders;

    int stepYStart = 0;
    int stepStartX = 480;

    CustomLookAndFeel customLookAndFeel;
    void handleFilterToggleLogic(int i, juce::TextButton& clickedButton);
    void setupToggleButton(juce::TextButton& button, const juce::String& text);

    std::array<juce::TextButton, NUM_SAMPLES> bitcrusherToggleButtons;
    std::array<juce::Slider, NUM_SAMPLES> bitDepthSliders;
    std::array<juce::Slider, NUM_SAMPLES> downsampleRateSliders;

    std::array<juce::Slider, NUM_SAMPLES> gainSliders;

    std::array<juce::Slider, NUM_SAMPLES> attackSliders;
    std::array<juce::Slider, NUM_SAMPLES> decaySliders;
    std::array<juce::Slider, NUM_SAMPLES> sustainSliders;
    std::array<juce::Slider, NUM_SAMPLES> releaseSliders;

    std::array<juce::Label, NUM_SAMPLES> gainLabels;
    std::array<juce::Label, NUM_SAMPLES * 4> adsrLabels;

    std::array<juce::GroupComponent, NUM_SAMPLES> sampleControlGroups;
    StepHighlightOverlay stepHighlightOverlay;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleAudioProcessorEditor)
};


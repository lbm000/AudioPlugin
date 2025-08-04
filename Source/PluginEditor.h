#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_core/juce_core.h>

#include "PluginProcessor.h"


static constexpr int NUM_TRACKS = SampleAudioProcessor::NUM_SAMPLES;
static constexpr int NUM_STEPS = 16;

/** @class ADSREditorComponent
 *  @brief Interactive visual component for editing ADSR envelopes.
 */
class ADSREditorComponent : public juce::Component
{
public:
    /** @brief Default constructor for the ADSR editor component. */
    ADSREditorComponent();

    /**
     * @brief Sets the ADSR envelope values.
     * @param attack Attack time.
     * @param decay Decay time.
     * @param sustain Sustain level.
     * @param release Release time.
     */
    void setAdsr(double attack, double decay, double sustain, double release);

    /** @brief Paints the ADSR envelope curve and handles. */
    void paint(juce::Graphics& g) override;

    /** @brief Handles mouse press events for dragging handles. */
    void mouseDown(const juce::MouseEvent& e) override;

    /** @brief Handles dragging of ADSR handles. */
    void mouseDrag(const juce::MouseEvent& e) override;

    /**
     * @brief Callback triggered when ADSR values change.
     * @param attack New attack value.
     * @param decay New decay value.
     * @param sustain New sustain value.
     * @param release New release value.
     */
    std::function<void(double attack, double decay, double sustain, double release)> onAdsrChanged;

private:
    /** @brief Current ADSR values. */
    double attack = 0.1, decay = 0.1, sustain = 0.8, release = 0.2;

    /** @brief Flags to indicate which handle is being dragged. */
    bool draggingAttack = false, draggingDecay = false, draggingSustain = false, draggingRelease = false;

    /**
     * @brief Draws a handle on the ADSR curve.
     * @param g Graphics context.
     * @param x X position of the handle.
     * @param y Y position of the handle.
     */
    void drawHandle(juce::Graphics& g, float x, float y);
};




/** @class CustomLookAndFeel
 *  @brief Custom LookAndFeel for rotary sliders and buttons.
 */
class CustomLookAndFeel : public juce::LookAndFeel_V4
{
public:
    /** @brief Constructor that sets custom color schemes. */
    CustomLookAndFeel()
    {
        setColour(juce::TextButton::buttonOnColourId, juce::Colours::deepskyblue);
    }

    /**
     * @brief Draws a custom rotary slider with tick marks and visual feedback.
     * @param g Graphics context.
     * @param x X coordinate of the slider.
     * @param y Y coordinate of the slider.
     * @param width Width of the slider.
     * @param height Height of the slider.
     * @param sliderPos Position of the slider (0.0 to 1.0).
     * @param rotaryStartAngle Starting angle of the rotary.
     * @param rotaryEndAngle Ending angle of the rotary.
     * @param slider Reference to the slider.
     */
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



/** @class StepButton
*  @brief Custom toggleable button used in the step sequencer.
*/
class StepButton : public juce::Button
{
public:
    /** @brief Constructor for the StepButton. */
    StepButton() : juce::Button("Step") {}


    /**
     * @brief Paints the button based on its toggle state and interaction.
     * @param g Graphics context.
     * @param shouldDrawButtonAsHighlighted Whether the button is highlighted.
     * @param shouldDrawButtonAsDown Whether the button is pressed down.
     */
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



/** @class StepHighlightOverlay
 *  @brief Visual overlay to indicate the currently active step in the sequencer.
 */
class StepHighlightOverlay : public juce::Component
{
public:
    /** @brief Paints the highlight overlay for the active step. */
    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colours::yellow.withAlpha(0.3f));
        g.fillRect(getLocalBounds());
    }
};





/** @class SampleAudioProcessorEditor
 *  @brief Main plugin editor class containing UI and sequencing logic.
 */
class SampleAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                       private juce::Timer
{
public:

    /**
     * @brief Constructs the plugin editor.
     * @param processor Reference to the audio processor.
     */
    SampleAudioProcessorEditor (SampleAudioProcessor&);

    /** @brief Destructor. */
    ~SampleAudioProcessorEditor() override;

    /** @brief Paints the plugin editor UI. */
    void paint (juce::Graphics&) override;

    /** @brief Resizes and repositions UI components. */
    void resized() override;

    /** @brief Timer callback used to advance the sequencer step. */
    void timerCallback() override;




private:

    /** @brief Reference to the associated audio processor. */
    SampleAudioProcessor& audioProcessor;

    static constexpr int NUM_SAMPLES = 5;


    /** @brief Buttons to load and play individual samples. */
    std::array<juce::TextButton, NUM_SAMPLES> loadSampleButtons;
    std::array<juce::TextButton, NUM_SAMPLES> playSampleButtons;

    /** @brief Slider and label for global BPM control. */
    juce::Slider globalBpmSlider;
    juce::Label globalBpmLabel;

    /** @brief File chooser for sample loading. */
    std::unique_ptr<juce::FileChooser> fileChooser;

    /** @brief Grid of step sequencer buttons. */
    StepButton stepButtons[NUM_TRACKS][NUM_STEPS];

    /** @brief Highlight overlay for the current sequencer step. */
    StepHighlightOverlay stepHighlightOverlay;

    /** @brief Labels above the steps for time indication. */
    std::array<juce::Label, NUM_STEPS> stepLabels;

    /** @brief Group component containing the sequencer. */
    juce::GroupComponent stepSequencerGroup;

    /** @brief Currently active step index. */
    int currentStep = 0;

    /** @brief Group components wrapping controls for each sample. */
    std::array<juce::GroupComponent, NUM_SAMPLES> sampleControlGroups;

    /** @brief Toggle buttons and sliders for filters (LPF, HPF, etc.). */
    juce::TextButton lpfToggleButtons[NUM_SAMPLES];
    juce::Slider lpfCutoffSliders[NUM_SAMPLES];
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
    std::array<juce::TextButton, NUM_SAMPLES> bitcrusherToggleButtons;
    std::array<juce::Slider, NUM_SAMPLES> bitDepthSliders;
    std::array<juce::Slider, NUM_SAMPLES> downsampleRateSliders;
    std::array<juce::Slider, NUM_SAMPLES> gainSliders;
    std::array<juce::Label, NUM_SAMPLES> gainLabels;


    int stepYStart = 0;
    int stepStartX = 480;


    /** @brief Custom LookAndFeel instance for the UI. */
    CustomLookAndFeel customLookAndFeel;

    /**
     * @brief Handles mutual exclusivity logic when a filter toggle button is pressed.
     * @param i Index of the sample.
     * @param clickedButton The button that was clicked.
     */
    void handleFilterToggleLogic(int i, juce::TextButton& clickedButton);

    /**
     * @brief Helper function to initialize a toggle button.
     * @param button Reference to the button.
     * @param text Button text.
     */
    void setupToggleButton(juce::TextButton& button, const juce::String& text);

    /** @brief ADSR editors for each sample. */
    std::array<std::unique_ptr<ADSREditorComponent>, NUM_SAMPLES> adsrEditors;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleAudioProcessorEditor)
};







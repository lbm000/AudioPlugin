#include "PluginProcessor.h"
#include "PluginEditor.h"

/**
 * @brief Constructs the editor for the SampleAudioProcessor.
 *
 * Initializes GUI components including sample controls, filters, bitcrusher,
 * gain sliders, ADSR editors, step sequencer, and global BPM controls.
 *
 * @param p Reference to the SampleAudioProcessor.
 */
SampleAudioProcessorEditor::SampleAudioProcessorEditor (SampleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&customLookAndFeel);

    /**
     * @brief Configures a given slider as a rotary knob.
     *
     * @param slider The slider to be configured.
     * @param suffix Optional suffix string to be displayed after the value.
     */
    auto configureAsKnob = [](juce::Slider& slider, const juce::String& suffix = "")
    {
        slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
        slider.setNumDecimalPlacesToDisplay(2);
        if (!suffix.isEmpty())
            slider.setTextValueSuffix(" " + suffix);
    };

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
         /**
          * @brief Sets up the visual group container for each sample's controls.
          */
        sampleControlGroups[i].setText("Sample " + juce::String(i + 1));
        sampleControlGroups[i].setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
        sampleControlGroups[i].setColour(juce::GroupComponent::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(sampleControlGroups[i]);

        /**
         * @brief Load button to choose a sample file.
         */
        loadSampleButtons[i].setButtonText("Load" + juce::String(i + 1));
        loadSampleButtons[i].onClick = [this, i]()
        {
            fileChooser = std::make_unique<juce::FileChooser>("Select a Sample", juce::File{}, "*.wav");
            fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this, i](const juce::FileChooser& chooser)
                {
                    auto file = chooser.getResult();
                    if (file.existsAsFile())
                        audioProcessor.loadSampleFile(file, i);
                });
        };
        addAndMakeVisible(loadSampleButtons[i]);


        /**
         * @brief Toggle button to play or pause the sample.
         */
        playSampleButtons[i].setClickingTogglesState(true);
        playSampleButtons[i].setButtonText("play");
        playSampleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        addAndMakeVisible(playSampleButtons[i]);

        playSampleButtons[i].onClick = [this, i]()
        {
            bool isPlaying = playSampleButtons[i].getToggleState();

            playSampleButtons[i].setButtonText(isPlaying ? "pause" : "play");
            playSampleButtons[i].setColour(juce::TextButton::buttonColourId,
                                           isPlaying ? juce::Colours::green : juce::Colours::darkgrey);

            audioProcessor.isSamplePlaying[i] = isPlaying;
        };

        /**
        * @brief Low-pass filter controls.
        */
        setupToggleButton(lpfToggleButtons[i], "LPF");
        configureAsKnob(lpfCutoffSliders[i], "Hz");
        lpfCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
        addAndMakeVisible(lpfCutoffSliders[i]);
        lpfCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setFilterCutoff(i, lpfCutoffSliders[i].getValue());
        };

        lpfToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = lpfToggleButtons[i].getToggleState();
            audioProcessor.setFilterEnabled(i, enabled);
        };

        /**
         * @brief High-pass filter controls.
         */
        setupToggleButton(highpassToggleButtons[i], "HPF");
        configureAsKnob(highpassCutoffSliders[i], "Hz");
        highpassCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
        addAndMakeVisible(highpassCutoffSliders[i]);
        highpassCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setHighpassCutoff(i, highpassCutoffSliders[i].getValue());
        };
        highpassToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = highpassToggleButtons[i].getToggleState();
            audioProcessor.setHighpassEnabled(i, enabled);
        };

        /**
         * @brief Band-pass filter controls.
         */
        setupToggleButton(bandpassToggleButtons[i], "BPF");
        configureAsKnob(bandpassCutoffSliders[i], "Hz");
        configureAsKnob(bandpassBandwidthSliders[i], "Hz");
        addAndMakeVisible(bandpassCutoffSliders[i]);
        addAndMakeVisible(bandpassBandwidthSliders[i]);
        bandpassCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBandPassCutoff(i, bandpassCutoffSliders[i].getValue());
        };

        bandpassBandwidthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBandPassBandwidth(i, bandpassBandwidthSliders[i].getValue());
        };

        bandpassToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = bandpassToggleButtons[i].getToggleState();
            audioProcessor.setBandPassEnabled(i, enabled);
            handleFilterToggleLogic(i, bandpassToggleButtons[i]);
        };


        /**
         * @brief Notch filter controls.
         */
        setupToggleButton(notchToggleButtons[i], "Notch");
        configureAsKnob(notchCutoffSliders[i], "Hz");
        notchCutoffSliders[i].setRange(20.0, 5000.0, 1.0);
        notchCutoffSliders[i].setValue(1000.0);
        configureAsKnob(notchBandwidthSliders[i], "Hz");
        notchBandwidthSliders[i].setRange(10.0, 1000.0, 1.0);
        notchBandwidthSliders[i].setValue(100.0);
        addAndMakeVisible(notchCutoffSliders[i]);
        addAndMakeVisible(notchBandwidthSliders[i]);
        notchCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setNotchCutoff(i, notchCutoffSliders[i].getValue());
        };

        notchBandwidthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setNotchBandwidth(i, notchBandwidthSliders[i].getValue());
        };
        notchToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = notchToggleButtons[i].getToggleState();
            audioProcessor.setNotchEnabled(i, enabled);
            handleFilterToggleLogic(i, notchToggleButtons[i]);
        };

        /**
         * @brief Peak (bell) filter controls.
         */
        setupToggleButton(peakToggleButtons[i], "Peak");
        configureAsKnob(peakCutoffSliders[i], "Hz");
        peakCutoffSliders[i].setRange(20.0, 10000.0, 1.0);
        configureAsKnob(peakGainSliders[i], "dB");
        peakGainSliders[i].setRange(-24.0, 24.0, 0.1);
        configureAsKnob(peakQSliders[i], "Q");
        peakQSliders[i].setRange(0.1, 10.0, 0.1);
        addAndMakeVisible(peakCutoffSliders[i]);
        addAndMakeVisible(peakGainSliders[i]);
        addAndMakeVisible(peakQSliders[i]);
        peakCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setPeakCutoff(i, peakCutoffSliders[i].getValue());
        };

        peakGainSliders[i].onValueChange = [this, i]() {
            audioProcessor.setPeakGain(i, peakGainSliders[i].getValue());
        };

        peakQSliders[i].onValueChange = [this, i]() {
            audioProcessor.setPeakQ(i, peakQSliders[i].getValue());
        };
        peakToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = peakToggleButtons[i].getToggleState();
            audioProcessor.setPeakEnabled(i, enabled);
            handleFilterToggleLogic(i, peakToggleButtons[i]);
        };


        /**
         * @brief Bitcrusher effect controls.
         */
        setupToggleButton(bitcrusherToggleButtons[i], "Bitcrusher");
        addAndMakeVisible(bitcrusherToggleButtons[i]);

        configureAsKnob(bitDepthSliders[i], "bit");
        bitDepthSliders[i].setRange(1, 24, 1);
        addAndMakeVisible(bitDepthSliders[i]);
        bitDepthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBitDepth(i, (int)bitDepthSliders[i].getValue());
        };
        bitcrusherToggleButtons[i].onClick = [this, i]() {
            bool enabled = bitcrusherToggleButtons[i].getToggleState();
            audioProcessor.setBitcrusherEnabled(i, enabled);
        };
        configureAsKnob(downsampleRateSliders[i], "x");
        downsampleRateSliders[i].setRange(1, 50, 1);
        addAndMakeVisible(downsampleRateSliders[i]);
        downsampleRateSliders[i].onValueChange = [this, i]() {
            audioProcessor.setDownsampleRate(i, (int)downsampleRateSliders[i].getValue());
        };

        /**
         * @brief Gain control for each sample.
         */
        configureAsKnob(gainSliders[i], "Gain");
        gainSliders[i].setRange(0.0f, 2.0f, 0.01f);
        gainSliders[i].setValue(1.0f);
        addAndMakeVisible(gainSliders[i]);
        gainLabels[i].setText("Gain", juce::dontSendNotification);
        gainLabels[i].setJustificationType(juce::Justification::centred);
        gainLabels[i].setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(gainLabels[i]);
        gainSliders[i].onValueChange = [this, i]() {
            audioProcessor.setGainLevel(i, (float)gainSliders[i].getValue());
        };

        /**
         * @brief ADSR envelope editor for each sample.
         */
        adsrEditors[i] = std::make_unique<ADSREditorComponent>();
        addAndMakeVisible(*adsrEditors[i]);

        adsrEditors[i]->onAdsrChanged = [this, i](double a, double d, double s, double r)
        {
            audioProcessor.setAdsrAttack(i, a);
            audioProcessor.setAdsrDecay(i, d);
            audioProcessor.setAdsrSustain(i, s);
            audioProcessor.setAdsrRelease(i, r);
        };


    }

    /**
     * @brief Initializes the step sequencer buttons.
     */
    for (int track = 0; track < NUM_TRACKS; ++track)
    {
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            auto& button = stepButtons[track][step];
            button.setClickingTogglesState(true);
            button.onClick = [this, track, step]
            {
                bool isOn = stepButtons[track][step].getToggleState();
                audioProcessor.setStepState(track, step, isOn);
            };
            addAndMakeVisible(button);
        }
    }

    /**
     * @brief Creates step number labels above the sequencer.
     */
    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setText(juce::String(step + 1), juce::dontSendNotification);
        stepLabels[step].setJustificationType(juce::Justification::centred);
        stepLabels[step].setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(stepLabels[step]);
    }


    /**
     * @brief Group box and label for the step sequencer.
     */
    stepSequencerGroup.setText("Step Sequencer");
    stepSequencerGroup.setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
    stepSequencerGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(stepSequencerGroup);

    /**
     * @brief Global BPM controls.
     */
    globalBpmLabel.setText("Global BPM", juce::dontSendNotification);
    globalBpmLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(globalBpmLabel);

    globalBpmSlider.setSliderStyle(juce::Slider::LinearVertical);
    globalBpmSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    globalBpmSlider.setNumDecimalPlacesToDisplay(0);
    globalBpmSlider.setRange(1.0, 300.0, 1.0);
    globalBpmSlider.setValue(audioProcessor.getGlobalBpm());
    globalBpmSlider.onValueChange = [this]() {
        audioProcessor.setGlobalBpm(globalBpmSlider.getValue());
    };
    addAndMakeVisible(globalBpmSlider);

    /**
     * @brief Overlay to highlight the current step in the sequencer.
     */
    addAndMakeVisible(stepHighlightOverlay);
    stepHighlightOverlay.toFront(true);

    /**
     * @brief Starts the timer to update the GUI regularly.
     */
    startTimerHz(10);


    /**
     * @brief Sets initial window size.
     */
    setSize(1800, 1000);
}


/**
 * @brief Destructor for the editor.
 *
 * Resets the look and feel to nullptr.
 */
SampleAudioProcessorEditor::~SampleAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

/**
 * Paints the GUI components of the SampleAudioProcessorEditor.
 *
 * This function fills the background with the default LookAndFeel colour.
 * If the step sequencer group is visible, it calculates the current step position
 * and paints a semi-transparent yellow rectangle over the active step column.
 *
 * @param g Reference to the graphics context used for rendering the GUI.
 */
void SampleAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    if (stepSequencerGroup.isVisible())
    {
        auto sequencerArea = stepSequencerGroup.getBounds();
        auto sequencerContentBounds = sequencerArea.reduced(10);
        auto labelHeight = 20;
        sequencerContentBounds.removeFromTop(labelHeight);

        int stepWidth = sequencerContentBounds.getWidth() / NUM_STEPS;
        int trackHeight = sequencerContentBounds.getHeight() / NUM_TRACKS;

        auto absX = sequencerContentBounds.getX() + (currentStep * stepWidth);
        auto absY = sequencerContentBounds.getY();

        g.setColour(juce::Colours::yellow.withAlpha(0.8f));
        g.fillRect(absX, absY, stepWidth, NUM_TRACKS * trackHeight);
    }
}



/**
 * @brief Resizes and arranges all GUI components of the plugin editor.
 *
 * This method defines the layout of all GUI components when the window is resized.
 * It includes layout logic for the step sequencer, BPM controls, and each sample's
 * individual control group, including gain, filters, bitcrusher and ADSR.
 *
 * Layout Overview:
 * - Top area: step sequencer and global BPM control.
 * - Remaining vertical space: per-sample control sections.
 *
 * Constants:
 * @var knobSize Size of each rotary control (slider).
 * @var spacing Spacing in pixels between components.
 * @var stepSeqHeight Fixed height for the step sequencer area.
 */
void SampleAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    int knobSize = 60;
    int spacing = 5;
    int stepSeqHeight = 300;

    /**
     * @brief Layout for the step sequencer and BPM controls.
     */
    auto topArea = bounds.removeFromTop(stepSeqHeight);

    int bpmWidth = 120;
    auto bpmArea = topArea.removeFromRight(bpmWidth);
    auto stepSequencerArea = topArea;

    stepSequencerGroup.setBounds(stepSequencerArea);
    auto bpmLabelHeight = 25;
    auto bpmSliderHeight = 250;

    globalBpmLabel.setBounds(bpmArea.removeFromTop(bpmLabelHeight).reduced(5));
    globalBpmSlider.setBounds(bpmArea.withSizeKeepingCentre(60, bpmSliderHeight));

    /**
     * @brief Layout for the step labels and step buttons.
     */
    auto sequencerContentBounds = stepSequencerGroup.getBounds().reduced(10);
    auto labelsArea = sequencerContentBounds.removeFromTop(20);

    int stepWidth = labelsArea.getWidth() / NUM_STEPS;
    int trackHeight = sequencerContentBounds.getHeight() / NUM_TRACKS;

    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setBounds(labelsArea.getX() + step * stepWidth,
                                   labelsArea.getY(),
                                   stepWidth,
                                   labelsArea.getHeight());
    }

    for (int track = 0; track < NUM_TRACKS; ++track)
    {
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            auto x = sequencerContentBounds.getX() + step * stepWidth;
            auto y = sequencerContentBounds.getY() + track * trackHeight;
            stepButtons[track][step].setBounds(x, y, stepWidth, trackHeight);
        }
    }

    /**
     * @brief Initializes the step highlight overlay with zero size.
     */
    stepHighlightOverlay.setBounds(0, 0, 0, 0);

    /**
     * @brief Layout for each sample control group.
     * Each group includes load/play buttons, gain, filters, bitcrusher and ADSR.
     */
    auto controlsArea = bounds;
    int groupHeight = controlsArea.getHeight() / NUM_SAMPLES;

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        auto groupBounds = controlsArea.removeFromTop(groupHeight)
            .withTrimmedRight(500)
            .withTrimmedTop(5)
            .withTrimmedBottom(5);
        sampleControlGroups[i].setBounds(groupBounds);

        auto contentArea = groupBounds.reduced(10);

        /**
         * @brief Sample Load and Play buttons
         */
        auto sampleControlsLeft = contentArea.removeFromLeft(knobSize * 2 + spacing * 2);
        sampleControlsLeft.removeFromTop(15);
        loadSampleButtons[i].setBounds(sampleControlsLeft.removeFromTop(30).withSizeKeepingCentre(knobSize, 25));
        sampleControlsLeft.removeFromTop(spacing);
        playSampleButtons[i].setBounds(sampleControlsLeft.removeFromTop(30).withSizeKeepingCentre(knobSize, 25));

        /**
         * @brief Gain control
         */
        contentArea.removeFromLeft(spacing);
        auto gainArea = contentArea.removeFromLeft(knobSize);
        gainLabels[i].setBounds(gainArea.removeFromTop(20));
        gainSliders[i].setBounds(gainArea);
        contentArea.removeFromLeft(spacing * 2);

        /**
         * @brief Filter and Bitcrusher section
         */
        auto filtersAndBitcrusherArea = contentArea.removeFromLeft(knobSize * 9 + spacing * 8);

        auto lpfArea = filtersAndBitcrusherArea.removeFromLeft(knobSize + spacing);
        lpfToggleButtons[i].setBounds(lpfArea.removeFromTop(20));
        lpfCutoffSliders[i].setBounds(lpfArea);

        auto hpfArea = filtersAndBitcrusherArea.removeFromLeft(knobSize + spacing);
        highpassToggleButtons[i].setBounds(hpfArea.removeFromTop(20));
        highpassCutoffSliders[i].setBounds(hpfArea);

        auto bpfArea = filtersAndBitcrusherArea.removeFromLeft(knobSize * 2 + spacing);
        bandpassToggleButtons[i].setBounds(bpfArea.removeFromTop(20));
        bandpassCutoffSliders[i].setBounds(bpfArea.removeFromLeft(knobSize));
        bandpassBandwidthSliders[i].setBounds(bpfArea);

        auto notchArea = filtersAndBitcrusherArea.removeFromLeft(knobSize * 2 + spacing);
        notchToggleButtons[i].setBounds(notchArea.removeFromTop(20));
        notchCutoffSliders[i].setBounds(notchArea.removeFromLeft(knobSize));
        notchBandwidthSliders[i].setBounds(notchArea);

        auto peakArea = filtersAndBitcrusherArea.removeFromLeft(knobSize * 3 + spacing * 2);
        peakToggleButtons[i].setBounds(peakArea.removeFromTop(20));
        peakCutoffSliders[i].setBounds(peakArea.removeFromLeft(knobSize));
        peakGainSliders[i].setBounds(peakArea.removeFromLeft(knobSize));
        peakQSliders[i].setBounds(peakArea.removeFromLeft(knobSize));

        auto bitcrusherArea = contentArea.removeFromLeft(knobSize * 2 + spacing);
        bitcrusherToggleButtons[i].setBounds(bitcrusherArea.removeFromTop(20));
        bitDepthSliders[i].setBounds(bitcrusherArea.removeFromLeft(knobSize));
        downsampleRateSliders[i].setBounds(bitcrusherArea);

        contentArea.removeFromLeft(spacing);

        /**
         * @brief ADSR Envelope Editor
         */
        auto adsrArea = contentArea;
        int adsrBaseIndex = i * 4;

        adsrEditors[i]->setBounds(adsrArea);
    }
}


/**
 * @brief Updates the step sequencer highlight overlay based on the current playback step.
 */
void SampleAudioProcessorEditor::timerCallback()
{
    currentStep = audioProcessor.getCurrentStep();

    auto sequencerArea = stepSequencerGroup.getBounds().reduced(10);
    auto labelHeight = 20;
    sequencerArea.removeFromTop(labelHeight);

    int stepWidth = sequencerArea.getWidth() / NUM_STEPS;
    int trackHeight = sequencerArea.getHeight() / NUM_TRACKS;

    int x = sequencerArea.getX() + currentStep * stepWidth;
    int y = sequencerArea.getY();
    int width = stepWidth;
    int height = NUM_TRACKS * trackHeight;

    stepHighlightOverlay.setBounds(x, y, width, height);
    stepHighlightOverlay.repaint();
}

/**
 * @brief Handles mutual exclusion logic between toggle buttons for different filter types.
 *
 * @param i The index of the sample/track.
 * @param clickedButton The toggle button that was clicked.
 */
void SampleAudioProcessorEditor::handleFilterToggleLogic(int i, juce::TextButton& clickedButton)
{
    if (&clickedButton == &bandpassToggleButtons[i])
    {
        peakToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        audioProcessor.setPeakEnabled(i, false);
    }
    else if (&clickedButton == &peakToggleButtons[i])
    {
        bandpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        audioProcessor.setBandPassEnabled(i, false);
    }

    if (&clickedButton == &notchToggleButtons[i])
    {
        lpfToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        highpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        bandpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        peakToggleButtons[i].setToggleState(false, juce::dontSendNotification);

        audioProcessor.setFilterEnabled(i, false);
        audioProcessor.setHighpassEnabled(i, false);
        audioProcessor.setBandPassEnabled(i, false);
        audioProcessor.setPeakEnabled(i, false);
    }
}


/**
 * @brief Configures a toggle button with default properties.
 *
 * @param button The toggle button to configure.
 * @param text The label text to set on the button.
 */
void SampleAudioProcessorEditor::setupToggleButton(juce::TextButton& button, const juce::String& text)
{
    button.setButtonText(text);
    button.setClickingTogglesState(true);
    addAndMakeVisible(button);
}


/**
 * @brief Constructor for the ADSREditorComponent.
 */
ADSREditorComponent::ADSREditorComponent()
{
    setMouseCursor(juce::MouseCursor::PointingHandCursor);
}


/**
 * @brief Sets the ADSR values and triggers a repaint.
 *
 * @param a Attack time.
 * @param d Decay time.
 * @param s Sustain level.
 * @param r Release time.
 */
void ADSREditorComponent::setAdsr(double a, double d, double s, double r)
{
    attack = a;
    decay = d;
    sustain = s;
    release = r;
    repaint();
}


/**
 * @brief Paints the ADSR envelope graph.
 *
 * This method renders a visual representation of the ADSR (Attack, Decay, Sustain, Release)
 * envelope over a background area. It calculates the relative positions of each phase based
 * on the total time and current parameter values, and draws the ADSR curve using a path.
 *
 * Additionally, it draws draggable handles at the key transition points:
 * end of Attack, level of Sustain, and end of Release.
 *
 * @param g The graphics context used for painting.
 */
void ADSREditorComponent::paint(juce::Graphics& g)
{
    /**
     * @brief The drawable area where the envelope will be rendered.
     * Reduced for visual margin.
     */
    auto area = getLocalBounds().toFloat().reduced(10.0f);

    g.setColour(juce::Colours::darkslategrey);
    g.fillRect(area);

    /**
     * @brief Total time includes attack, decay, and release durations.
     * A small offset is added to prevent division by zero.
     */
    float totalTime = attack + decay + release + 0.001f;

    /**
     * @brief Calculate key points of the ADSR envelope in screen coordinates.
     */
    float attackX = area.getX();
    float attackY = area.getBottom();

    float decayX = attackX + (attack / totalTime) * area.getWidth();
    float decayY = area.getY();

    float sustainX = decayX + (decay / totalTime) * area.getWidth();
    float sustainY = area.getY() + (1.0f - sustain) * area.getHeight();

    float releaseX = sustainX + (release / totalTime) * area.getWidth();
    float releaseY = area.getBottom();

    /**
     * @brief Construct the ADSR path connecting attack, decay, sustain, and release.
     */
    juce::Path adsrPath;
    adsrPath.startNewSubPath(attackX, attackY);     /**< Start at bottom-left (attack start) */
    adsrPath.lineTo(decayX, decayY);                /**< Line up to decay peak */
    adsrPath.lineTo(sustainX, sustainY);            /**< Line down to sustain level */
    adsrPath.lineTo(releaseX, releaseY);            /**< Line down to release end */

    g.setColour(juce::Colours::deepskyblue);
    g.strokePath(adsrPath, juce::PathStrokeType(2.0f));

    /**
     * @brief Draw draggable handles at the transition points of the envelope.
     *
     * @note Handles may be used for interactive editing (attack end, sustain level, release end).
     */
    drawHandle(g, decayX, decayY);       /**< Handle for end of Attack phase */
    drawHandle(g, sustainX, sustainY);   /**< Handle for Sustain level */
    drawHandle(g, releaseX, releaseY);   /**< Handle for end of Release phase */
}



/**
 * @brief Handles mouse down events to determine which handle is being dragged.
 *
 * This function checks if the mouse click occurred near one of the draggable points
 * of the ADSR envelope graph. If the click is within a certain threshold distance from
 * a handle (end of attack, sustain level, or end of release), it activates the corresponding
 * dragging flag.
 *
 * @param e The mouse event data, including position of the click.
 */
void ADSREditorComponent::mouseDown(const juce::MouseEvent& e)
{
    /**
     * @brief Calculate the drawable area of the component, with padding.
     */
    auto area = getLocalBounds().toFloat().reduced(10.0f);

    /**
     * @brief Compute the total ADSR time to normalize the X coordinates.
     * A small epsilon is added to prevent division by zero.
     */
    float totalTime = attack + decay + release + 0.001f;

    /**
     * @brief Calculate X/Y positions of the key ADSR points.
     */
    float attackX = area.getX();
    float decayX = attackX + (attack / totalTime) * area.getWidth();
    float decayY = area.getY();

    float sustainX = decayX + (decay / totalTime) * area.getWidth();
    float sustainY = area.getY() + (1.0f - sustain) * area.getHeight();

    float releaseX = sustainX + (release / totalTime) * area.getWidth();
    float releaseY = area.getBottom();

    /**
     * @brief The position of the mouse click.
     */
    juce::Point<float> click = e.position;

    /**
     * @brief Lambda function to check if the click is near a handle.
     *
     * @param p1 Position of the click.
     * @param p2 Position of the handle.
     * @param threshold Maximum distance to consider "near".
     * @return true if p1 is within threshold of p2.
     */
    auto isNear = [](juce::Point<float> p1, juce::Point<float> p2, float threshold = 10.0f)
    {
        return p1.getDistanceFrom(p2) <= threshold;
    };

    /**
     * @brief Determine which handle is being dragged, based on click proximity.
     *
     * Priority:
     * - Attack (decay start point)
     * - Decay/Sustain (same point, sets both flags)
     * - Release (end point)
     */
    draggingAttack  = isNear(click, { decayX, decayY });
    draggingDecay   = !draggingAttack && isNear(click, { sustainX, sustainY });
    draggingSustain = draggingDecay;
    draggingRelease = !draggingAttack && !draggingDecay && isNear(click, { releaseX, releaseY });
}



/**
 * @brief Handles dragging of ADSR points and updates the values accordingly.
 *
 * This function is called continuously as the user drags the mouse after clicking
 * near a handle (attack end, sustain level, or release end). It converts the mouse
 * position into relative values within the ADSR graph and updates the corresponding
 * ADSR parameters. The values are clamped to avoid invalid ranges, and a callback
 * is triggered when the values change.
 *
 * @param e The mouse event data, including the current mouse position.
 */
void ADSREditorComponent::mouseDrag(const juce::MouseEvent& e)
{
    /**
     * @brief Define the drawable bounds of the ADSR area.
     */
    auto area = getLocalBounds().toFloat().reduced(10.0f);
    float width = area.getWidth();
    float height = area.getHeight();

    /**
     * @brief Compute the total time to normalize X-axis values.
     */
    float totalTime = attack + decay + release + 0.001f;

    /**
     * @brief Convert the mouse position into relative [0, 1] coordinates.
     * relX corresponds to time position, relY to level (inverted Y axis).
     */
    float relX = (e.position.x - area.getX()) / width;
    float relY = 1.0f - ((e.position.y - area.getY()) / height);

    /**
     * @brief Clamp relative coordinates to the valid range [0.0, 1.0].
     */
    relX = juce::jlimit(0.0f, 1.0f, relX);
    relY = juce::jlimit(0.0f, 1.0f, relY);

    /**
     * @brief Convert relative X position into absolute time based on total ADSR time.
     */
    float newTime = relX * totalTime;

    /**
     * @brief Update ADSR values depending on which handle is being dragged.
     * Values are clamped to ensure the ADSR shape remains valid.
     */
    if (draggingAttack)
    {
        attack = juce::jlimit(0.01f, static_cast<float>(totalTime - decay - release), static_cast<float>(newTime));
    }
    else if (draggingDecay)
    {
        release = juce::jlimit<float>(
            0.01f,
            static_cast<float>(totalTime - attack - decay),
            static_cast<float>(newTime - attack - decay)
        );        sustain = juce::jlimit(0.0f, 1.0f, relY);
    }
    else if (draggingRelease)
    {
        release = juce::jlimit<float>(
            0.01f,
            static_cast<float>(totalTime - attack - decay),
            static_cast<float>(newTime - attack - decay)
        );
    }

    /**
     * @brief Notify listeners if a callback is registered for ADSR changes.
     */
    if (onAdsrChanged)
        onAdsrChanged(attack, decay, sustain, release);

    /**
     * @brief Repaint the component to reflect the updated envelope.
     */
    repaint();
}


/**
 * @brief Draws a small circular handle at the given position.
 *
 * This function is used to visually represent a draggable control point
 * on the ADSR envelope graph. The handle is drawn as a filled white circle,
 * centered at the given (x, y) position.
 *
 * @param g The graphics context used to draw the handle.
 * @param x The x-coordinate of the center of the handle.
 * @param y The y-coordinate of the center of the handle.
 */
void ADSREditorComponent::drawHandle(juce::Graphics& g, float x, float y)
{
    g.setColour(juce::Colours::white);
    g.fillEllipse(x - 4.0f, y - 4.0f, 8.0f, 8.0f);
}


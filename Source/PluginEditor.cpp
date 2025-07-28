#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
//==============================================================================
SampleAudioProcessorEditor::SampleAudioProcessorEditor (SampleAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setLookAndFeel(&customLookAndFeel);

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
        // --- LOAD / PLAY ---
        loadSampleButtons[i].setButtonText("Load " + juce::String(i + 1));
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

        playSampleButtons[i].setClickingTogglesState(true);
        playSampleButtons[i].setButtonText("▶");
        playSampleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        addAndMakeVisible(playSampleButtons[i]);

        playSampleButtons[i].onClick = [this, i]()
        {
            bool isPlaying = playSampleButtons[i].getToggleState();

            playSampleButtons[i].setButtonText(isPlaying ? "■" : "▶");
            playSampleButtons[i].setColour(juce::TextButton::buttonColourId,
                                           isPlaying ? juce::Colours::green : juce::Colours::darkgrey);

            audioProcessor.isSamplePlaying[i] = isPlaying;
        };

        // --- LPF ---
        setupToggleButton(filterToggleButtons[i], "LPF");
        configureAsKnob(filterCutoffSliders[i], "Hz");
        filterCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
        addAndMakeVisible(filterCutoffSliders[i]);
        filterCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setFilterCutoff(i, filterCutoffSliders[i].getValue());
        };

        // --- HPF ---
        setupToggleButton(highpassToggleButtons[i], "HPF");
        configureAsKnob(highpassCutoffSliders[i], "Hz");
        highpassCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
        addAndMakeVisible(highpassCutoffSliders[i]);
        highpassCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setHighpassCutoff(i, highpassCutoffSliders[i].getValue());
        };

        // --- BPF ---
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


        // --- Notch ---
        setupToggleButton(notchToggleButtons[i], "Notch");
        configureAsKnob(notchCutoffSliders[i], "Hz");
        configureAsKnob(notchBandwidthSliders[i], "Hz");
        addAndMakeVisible(notchCutoffSliders[i]);
        addAndMakeVisible(notchBandwidthSliders[i]);
        notchCutoffSliders[i].onValueChange = [this, i]() {
            audioProcessor.setNotchCutoff(i, notchCutoffSliders[i].getValue());
        };

        notchBandwidthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setNotchBandwidth(i, notchBandwidthSliders[i].getValue());
        };

        // --- Peak ---
        setupToggleButton(peakToggleButtons[i], "Peak");
        configureAsKnob(peakCutoffSliders[i], "Hz");
        configureAsKnob(peakGainSliders[i], "dB");
        configureAsKnob(peakQSliders[i], "Q");
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

        // --- Bitcrusher ---
        setupToggleButton(bitcrusherToggleButtons[i], "Bitcrusher");
        addAndMakeVisible(bitcrusherToggleButtons[i]);

        configureAsKnob(bitDepthSliders[i], "bit");
        bitDepthSliders[i].setRange(1, 24, 1);
        addAndMakeVisible(bitDepthSliders[i]);
        bitDepthSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBitDepth(i, (int)bitDepthSliders[i].getValue());
        };

        //gain
        configureAsKnob(gainSliders[i], "Gain");
        gainSliders[i].setRange(0.0f, 2.0f, 0.01f);
        gainSliders[i].setValue(1.0f);
        addAndMakeVisible(gainSliders[i]);

        gainSliders[i].onValueChange = [this, i]() {
            audioProcessor.setGainLevel(i, (float)gainSliders[i].getValue());
        };

        configureAsKnob(downsampleRateSliders[i], "x");
        downsampleRateSliders[i].setRange(1, 50, 1);
        addAndMakeVisible(downsampleRateSliders[i]);
        downsampleRateSliders[i].onValueChange = [this, i]() {
            audioProcessor.setDownsampleRate(i, (int)downsampleRateSliders[i].getValue());
        };

        bitcrusherToggleButtons[i].onClick = [this, i]() {
            bool enabled = bitcrusherToggleButtons[i].getToggleState();
            audioProcessor.setBitcrusherEnabled(i, enabled);
        };



        filterToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = filterToggleButtons[i].getToggleState();
            audioProcessor.setFilterEnabled(i, enabled);
        };

        highpassToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = highpassToggleButtons[i].getToggleState();
            audioProcessor.setHighpassEnabled(i, enabled);
        };

        bandpassToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = bandpassToggleButtons[i].getToggleState();
            audioProcessor.setBandPassEnabled(i, enabled);
            handleFilterToggleLogic(i, bandpassToggleButtons[i]);
        };

        notchToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = notchToggleButtons[i].getToggleState();
            audioProcessor.setNotchEnabled(i, enabled);
            handleFilterToggleLogic(i, notchToggleButtons[i]);
        };

        peakToggleButtons[i].onClick = [this, i]()
        {
            bool enabled = peakToggleButtons[i].getToggleState();
            audioProcessor.setPeakEnabled(i, enabled);
            handleFilterToggleLogic(i, peakToggleButtons[i]);
        };
    }


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

    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setText(juce::String(step + 1), juce::dontSendNotification);
        stepLabels[step].setJustificationType(juce::Justification::centred);
        stepLabels[step].setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(stepLabels[step]);
    }

    stepSequencerGroup.setText("Step Sequencer");
    stepSequencerGroup.setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
    stepSequencerGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(stepSequencerGroup);

    globalBpmLabel.setText("Global BPM", juce::dontSendNotification);
    globalBpmLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(globalBpmLabel);

    configureAsKnob(globalBpmSlider);
    globalBpmSlider.setRange(1.0, 300.0, 1.0);
    globalBpmSlider.setValue(audioProcessor.getGlobalBpm());
    globalBpmSlider.onValueChange = [this]() {
        audioProcessor.setGlobalBpm(globalBpmSlider.getValue());
    };
    addAndMakeVisible(globalBpmSlider);

    startTimerHz(10);

    setSize(1800, 700);
}


SampleAudioProcessorEditor::~SampleAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void SampleAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    const int stepSize = 25;
    const int rowHeight = 35;

    const int x = stepStartX + audioProcessor.getCurrentStep() * stepSize;
    const int y = stepYStart;

    g.setColour(juce::Colours::yellow.withAlpha(0.3f));
    g.fillRect(x, y, stepSize, NUM_TRACKS * rowHeight);
}



void SampleAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);


    int sequencerWidth = NUM_STEPS * 25 + 40;

    int totalWidth = getWidth();
    int minSequencerWidth = 600;
    int maxControlsWidth = totalWidth - minSequencerWidth;

    int controlsWidth = juce::jmin(150 + 6 * 240, maxControlsWidth);
    auto controlsArea = bounds.removeFromLeft(controlsWidth);
    auto sequencerArea = bounds;


    stepSequencerGroup.setBounds(sequencerArea);

    int buttonSize = 20;
    int stepSize = 25;
    int rowHeight = 35;
    int groupBorder = 15;

    stepStartX = stepSequencerGroup.getX() + groupBorder;
    stepYStart = stepSequencerGroup.getY() + groupBorder + 20;


    auto sequencerContent = sequencerArea.reduced(groupBorder);

    auto labelsArea = sequencerContent.removeFromTop(20);
    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setBounds(labelsArea.removeFromLeft(stepSize));
    }

    for (int i = 0; i < NUM_TRACKS; ++i)
    {
        auto trackArea = sequencerContent.removeFromTop(rowHeight);
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            stepButtons[i][step].setBounds(trackArea.removeFromLeft(stepSize).withSizeKeepingCentre(buttonSize, buttonSize));
        }
    }



    auto globalArea = controlsArea.removeFromTop(100);
    auto bpmArea = globalArea.removeFromLeft(100);

    globalBpmLabel.setBounds(bpmArea.removeFromTop(20));
    globalBpmSlider.setBounds(bpmArea);


    int trackControlHeight = 120;

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        auto trackBounds = controlsArea.removeFromTop(trackControlHeight);


        auto createKnobSection = [&](juce::TextButton& toggle, juce::Slider& knob1, juce::Slider* knob2 = nullptr, juce::Slider* knob3 = nullptr)
        {
            int sectionWidth = 80;
            if (knob2) sectionWidth += 80;
            if (knob3) sectionWidth += 80;

            auto section = trackBounds.removeFromLeft(sectionWidth);
            toggle.setBounds(section.removeFromTop(25).reduced(5));
            auto knobArea = section;

            knob1.setBounds(knobArea.removeFromLeft(80));
            if (knob2) knob2->setBounds(knobArea.removeFromLeft(80));
            if (knob3) knob3->setBounds(knobArea.removeFromLeft(80));
        };

        auto sampleControls = trackBounds.removeFromLeft(150);
        loadSampleButtons[i].setBounds(sampleControls.removeFromTop(30).reduced(5));
        playSampleButtons[i].setBounds(sampleControls.removeFromTop(30).reduced(5));

        createKnobSection(filterToggleButtons[i], filterCutoffSliders[i]);
        createKnobSection(highpassToggleButtons[i], highpassCutoffSliders[i]);
        createKnobSection(bandpassToggleButtons[i], bandpassCutoffSliders[i], &bandpassBandwidthSliders[i]);
        createKnobSection(notchToggleButtons[i], notchCutoffSliders[i], &notchBandwidthSliders[i]);
        createKnobSection(peakToggleButtons[i], peakCutoffSliders[i], &peakGainSliders[i], &peakQSliders[i]);
        createKnobSection(bitcrusherToggleButtons[i], bitDepthSliders[i], &downsampleRateSliders[i]);

        auto gainSection = trackBounds.removeFromLeft(80);
        gainSliders[i].setBounds(gainSection.reduced(5));
    }
}


void SampleAudioProcessorEditor::timerCallback()
{
    currentStep = audioProcessor.getCurrentStep();
    repaint();
}

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
        filterToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        highpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        bandpassToggleButtons[i].setToggleState(false, juce::dontSendNotification);
        peakToggleButtons[i].setToggleState(false, juce::dontSendNotification);

        audioProcessor.setFilterEnabled(i, false);
        audioProcessor.setHighpassEnabled(i, false);
        audioProcessor.setBandPassEnabled(i, false);
        audioProcessor.setPeakEnabled(i, false);
    }
}

void SampleAudioProcessorEditor::setupToggleButton(juce::TextButton& button, const juce::String& text)
{
    button.setButtonText(text);
    button.setClickingTogglesState(true);
    addAndMakeVisible(button);
}

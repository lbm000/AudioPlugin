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

        sampleControlGroups[i].setText("Sample " + juce::String(i + 1));
        sampleControlGroups[i].setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
        sampleControlGroups[i].setColour(juce::GroupComponent::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(sampleControlGroups[i]);

        // --- LOAD / PLAY ---
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
        gainLabels[i].setText("Gain", juce::dontSendNotification);
        gainLabels[i].setJustificationType(juce::Justification::centred);
        gainLabels[i].setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
        addAndMakeVisible(gainLabels[i]);


        gainSliders[i].onValueChange = [this, i]() {
            audioProcessor.setGainLevel(i, (float)gainSliders[i].getValue());
        };

        // --- ADSR ---
        configureAsKnob(attackSliders[i], "A");
        attackSliders[i].setRange(0.001, 5.0, 0.001);
        addAndMakeVisible(attackSliders[i]);
        attackSliders[i].onValueChange = [this, i]() {
            audioProcessor.setAdsrAttack(i, attackSliders[i].getValue());
        };

        configureAsKnob(decaySliders[i], "D");
        decaySliders[i].setRange(0.001, 5.0, 0.001);
        addAndMakeVisible(decaySliders[i]);
        decaySliders[i].onValueChange = [this, i]() {
            audioProcessor.setAdsrDecay(i, decaySliders[i].getValue());
        };

        configureAsKnob(sustainSliders[i], "S");
        sustainSliders[i].setRange(0.0, 1.0, 0.01);
        addAndMakeVisible(sustainSliders[i]);
        sustainSliders[i].onValueChange = [this, i]() {
            audioProcessor.setAdsrSustain(i, sustainSliders[i].getValue());
        };

        configureAsKnob(releaseSliders[i], "R");
        releaseSliders[i].setRange(0.001, 5.0, 0.001);
        addAndMakeVisible(releaseSliders[i]);
        int adsrBaseIndex = i * 4;
        const char* adsrNames[] = { "A", "D", "S", "R" };

        for (int j = 0; j < 4; ++j)
        {
            adsrLabels[adsrBaseIndex + j].setText(adsrNames[j], juce::dontSendNotification);
            adsrLabels[adsrBaseIndex + j].setJustificationType(juce::Justification::centred);
            adsrLabels[adsrBaseIndex + j].setColour(juce::Label::textColourId, juce::Colours::whitesmoke);
            addAndMakeVisible(adsrLabels[adsrBaseIndex + j]);
        }

        releaseSliders[i].onValueChange = [this, i]() {
            audioProcessor.setAdsrRelease(i, releaseSliders[i].getValue());
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

    globalBpmSlider.setSliderStyle(juce::Slider::LinearVertical);
    globalBpmSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 80, 20);
    globalBpmSlider.setNumDecimalPlacesToDisplay(0);
    globalBpmSlider.setRange(1.0, 300.0, 1.0);
    globalBpmSlider.setValue(audioProcessor.getGlobalBpm());
    globalBpmSlider.onValueChange = [this]() {
        audioProcessor.setGlobalBpm(globalBpmSlider.getValue());
    };
    addAndMakeVisible(globalBpmSlider);
    addAndMakeVisible(stepHighlightOverlay);
    stepHighlightOverlay.toFront(true);

    startTimerHz(10);

    setSize(1800, 1000);
}


SampleAudioProcessorEditor::~SampleAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
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



void SampleAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(10);

    int knobSize = 60;
    int spacing = 5;
    int stepSeqHeight = 200;
    int rightColumnWidth = 120;


    auto topArea = bounds.removeFromTop(stepSeqHeight);

    int bpmWidth = 120;
    auto bpmArea = topArea.removeFromRight(bpmWidth);
    auto stepSequencerArea = topArea;

    stepSequencerGroup.setBounds(stepSequencerArea);
    globalBpmLabel.setBounds(bpmArea.removeFromTop(20).withTrimmedLeft(10));
    globalBpmSlider.setBounds(bpmArea.withTrimmedLeft(10).withSizeKeepingCentre(60, 100));


    auto sequencerContentBounds = stepSequencerGroup.getBounds().reduced(10);    auto labelsArea = sequencerContentBounds.removeFromTop(20);

    int stepWidth = labelsArea.getWidth() / NUM_STEPS;
    int trackHeight = (sequencerContentBounds.getHeight()) / NUM_TRACKS;

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
    stepHighlightOverlay.setBounds(0, 0, 0, 0);
    addAndMakeVisible(stepHighlightOverlay);

    auto controlsArea = bounds;
    int groupHeight = controlsArea.getHeight() / NUM_SAMPLES;

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        // here i put the space at the right side of group with withTrimmedRight
        auto groupBounds = controlsArea.removeFromTop(groupHeight).withTrimmedRight(500).withTrimmedTop(5).withTrimmedBottom(5);
        sampleControlGroups[i].setBounds(groupBounds);

        auto contentArea = groupBounds.reduced(10);

        auto sampleControlsLeft = contentArea.removeFromLeft(knobSize * 2 + spacing * 2);
        sampleControlsLeft.removeFromTop(15);
        loadSampleButtons[i].setBounds(sampleControlsLeft.removeFromTop(30).withSizeKeepingCentre(knobSize, 25));
        sampleControlsLeft.removeFromTop(spacing);
        playSampleButtons[i].setBounds(sampleControlsLeft.removeFromTop(30).withSizeKeepingCentre(knobSize, 25));

        contentArea.removeFromLeft(spacing);
        auto gainArea = contentArea.removeFromLeft(knobSize);
        gainLabels[i].setBounds(gainArea.removeFromTop(20));
        gainSliders[i].setBounds(gainArea);
        contentArea.removeFromLeft(spacing * 2);

        auto filtersAndBitcrusherArea = contentArea.removeFromLeft(knobSize * 9 + spacing * 8);

        auto lpfArea = filtersAndBitcrusherArea.removeFromLeft(knobSize + spacing);
        filterToggleButtons[i].setBounds(lpfArea.removeFromTop(20));
        filterCutoffSliders[i].setBounds(lpfArea);

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

        auto adsrArea = contentArea;
        int adsrBaseIndex = i * 4;

        for (int j = 0; j < 4; ++j)
        {
            auto slot = adsrArea.removeFromLeft(knobSize);
            adsrLabels[adsrBaseIndex + j].setBounds(slot.removeFromTop(20));
            switch (j)
            {
            case 0: attackSliders[i].setBounds(slot); break;
            case 1: decaySliders[i].setBounds(slot); break;
            case 2: sustainSliders[i].setBounds(slot); break;
            case 3: releaseSliders[i].setBounds(slot); break;
            }
            adsrArea.removeFromLeft(spacing);
        }

    }
}


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

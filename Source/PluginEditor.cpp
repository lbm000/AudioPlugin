




/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AnimalBeatAudioProcessorEditor::AnimalBeatAudioProcessorEditor (AnimalBeatAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    for (int i = 0; i < NUM_ANIMALS; ++i)
{
    loadAnimalButtons[i].setButtonText("Load Animal " + juce::String(i + 1));
    loadAnimalButtons[i].onClick = [this, i]()
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select animal sound", juce::File{}, "*.wav;*.mp3");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this, i](const juce::FileChooser& fc)
            {
                juce::File selectedFile = fc.getResult();
                audioProcessor.loadAnimalFile(selectedFile, i);
            });
    };
    addAndMakeVisible(loadAnimalButtons[i]);

    playAnimalButtons[i].setButtonText("Play Animal " + juce::String(i + 1));
    playAnimalButtons[i].onClick = [this, i]() {
        audioProcessor.isAnimalPlaying[i] = !audioProcessor.isAnimalPlaying[i];

        auto isOn = audioProcessor.isAnimalPlaying[i];
        playAnimalButtons[i].setColour(juce::TextButton::buttonColourId, isOn ? juce::Colours::blue : juce::Colours::red);
    };
    playAnimalButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
    addAndMakeVisible(playAnimalButtons[i]);

    // Low-Pass Filter toggle
    filterToggleButtons[i].setButtonText("LPF " + juce::String(i + 1));
    filterToggleButtons[i].onClick = [this, i]() {
        bool isOn = !audioProcessor.getFilterEnabled(i);
        audioProcessor.setFilterEnabled(i, isOn);
        filterToggleButtons[i].setColour(juce::TextButton::buttonColourId,
            isOn ? juce::Colours::green : juce::Colours::darkgrey);
    };
    filterToggleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addAndMakeVisible(filterToggleButtons[i]);

    // Low-Pass Filter cutoff slider
    filterCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
    filterCutoffSliders[i].setValue(1000.0);
    filterCutoffSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    filterCutoffSliders[i].onValueChange = [this, i]() {
        audioProcessor.setFilterCutoff(i, filterCutoffSliders[i].getValue());
    };
    addAndMakeVisible(filterCutoffSliders[i]);

    // High-Pass Filter toggle
    highpassToggleButtons[i].setButtonText("HPF " + juce::String(i + 1));
    highpassToggleButtons[i].onClick = [this, i]() {
        bool isOn = !audioProcessor.getHighpassEnabled(i);
        audioProcessor.setHighpassEnabled(i, isOn);
        highpassToggleButtons[i].setColour(juce::TextButton::buttonColourId,
            isOn ? juce::Colours::green : juce::Colours::darkgrey);
    };
    highpassToggleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addAndMakeVisible(highpassToggleButtons[i]);

    // High-Pass Filter cutoff slider
    highpassCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
    highpassCutoffSliders[i].setValue(1000.0);
    highpassCutoffSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    highpassCutoffSliders[i].onValueChange = [this, i]() {
        audioProcessor.setHighpassCutoff(i, highpassCutoffSliders[i].getValue());
    };
    addAndMakeVisible(highpassCutoffSliders[i]);


    // Band-Pass Filter toggle
    bandpassToggleButtons[i].setButtonText("BPF " + juce::String(i + 1));
    bandpassToggleButtons[i].onClick = [this, i]() {
        bool isOn = audioProcessor.getBandPassEnabled(i);
        audioProcessor.setBandPassEnabled(i, isOn);

        if (isOn) {
            // Deactivate LPF and HPF
            audioProcessor.setFilterEnabled(i, false);
            audioProcessor.setHighpassEnabled(i, false);
            filterToggleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
            highpassToggleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
        }

        bandpassToggleButtons[i].setColour(juce::TextButton::buttonColourId,
            isOn ? juce::Colours::orange : juce::Colours::darkgrey);
    };
    bandpassToggleButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);
    addAndMakeVisible(bandpassToggleButtons[i]);


    // Band-Pass Filter Cutoff
    bandpassCutoffSliders[i].setRange(100.0, 10000.0, 1.0);
    bandpassCutoffSliders[i].setValue(1000.0);
    bandpassCutoffSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    bandpassCutoffSliders[i].onValueChange = [this, i]() {
        audioProcessor.setBandPassCutoff(i, bandpassCutoffSliders[i].getValue());
    };
    addAndMakeVisible(bandpassCutoffSliders[i]);

    // Bandwidth
    bandpassBandwidthSliders[i].setRange(10.0, 5000.0, 1.0);
    bandpassBandwidthSliders[i].setValue(1000.0);
    bandpassBandwidthSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    bandpassBandwidthSliders[i].onValueChange = [this, i]() {
        audioProcessor.setBandPassBandwidth(i, bandpassBandwidthSliders[i].getValue());
    };
    addAndMakeVisible(bandpassBandwidthSliders[i]);
}

    for (int i = 0; i < NUM_BEATS; ++i)
    {
        const int labelIndex = NUM_ANIMALS + i;

        loadBeatButtons[i].setButtonText("Load Beat " + juce::String(i + 1));
        loadBeatButtons[i].onClick = [this, i]()
        {
            fileChooser = std::make_unique<juce::FileChooser>("Select beat", juce::File{}, "*.wav;*.mp3");
            fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
                [this, i](const juce::FileChooser& fc)
                {
                    juce::File selectedFile = fc.getResult();
                    audioProcessor.loadBeatFile(selectedFile, i);
                });
        };
        addAndMakeVisible(loadBeatButtons[i]);

        playBeatButtons[i].setButtonText("Play Beat " + juce::String(i + 1));
        playBeatButtons[i].onClick = [this, i]() {
            audioProcessor.isBeatPlaying[i] = !audioProcessor.isBeatPlaying[i];

            auto isOn = audioProcessor.isBeatPlaying[i];
            playBeatButtons[i].setColour(juce::TextButton::buttonColourId, isOn ? juce::Colours::blue : juce::Colours::red);
        };
        playBeatButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::red);
        addAndMakeVisible(playBeatButtons[i]);

    }

    for (int track = 0; track < NUM_TRACKS; ++track)
    {
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            auto& button = stepButtons[track][step];

            button.setClickingTogglesState(true);  // button on/off

            button.onClick = [this, track, step]()
            {
                bool isOn = stepButtons[track][step].getToggleState();
                audioProcessor.setStepState(track, step, isOn);


                stepButtons[track][step].setColour(
                    juce::TextButton::buttonColourId,
                    isOn ? juce::Colours::green : juce::Colours::darkgrey
                );
            };


            button.setColour(juce::TextButton::buttonColourId, juce::Colours::darkgrey);

            addAndMakeVisible(button);
        }
    }



    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setText(juce::String(step + 1), juce::dontSendNotification);
        stepLabels[step].setJustificationType(juce::Justification::centred);
        stepLabels[step].setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(stepLabels[step]);
    }


    stepSequencerGroup.setText("Step Sequencer");
    stepSequencerGroup.setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
    stepSequencerGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(stepSequencerGroup);


    globalBpmLabel.setText("Global BPM", juce::dontSendNotification);
    globalBpmLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(globalBpmLabel);

    globalBpmSlider.setRange(1.0, 300.0, 1.0);
    globalBpmSlider.setValue(audioProcessor.getGlobalBpm());
    globalBpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    globalBpmSlider.onValueChange = [this]() {
        audioProcessor.setGlobalBpm(globalBpmSlider.getValue());
    };
    addAndMakeVisible(globalBpmSlider);

    startTimerHz(10);

    setSize(500, 350);
}


AnimalBeatAudioProcessorEditor::~AnimalBeatAudioProcessorEditor()
{
}

//==============================================================================
void AnimalBeatAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));


    int stepSize = 25;
    int buttonSize = 20;
    int stepStartX = 480;
    int stepYStart = 45 + NUM_ANIMALS * 70 + NUM_BEATS * 35 + 20;
    int rowHeight = 35;

    int x = stepStartX + audioProcessor.getCurrentStep() * stepSize;

    g.setColour(juce::Colours::yellow.withAlpha(0.3f));
    g.fillRect(x, stepYStart + 20, stepSize, NUM_TRACKS * rowHeight);
}


void AnimalBeatAudioProcessorEditor::resized()
{
    globalBpmLabel.setBounds(10, 10, 100, 25);
    globalBpmSlider.setBounds(110, 10, 150, 25);

    int y = 45;

    for (int i = 0; i < NUM_ANIMALS; ++i)
    {
        // Linha 1: Load, Play, LPF, LPF-Cutoff, HPF, HPF-Cutoff
        loadAnimalButtons[i].setBounds(10, y, 140, 25);
        playAnimalButtons[i].setBounds(160, y, 120, 25);
        filterToggleButtons[i].setBounds(290, y, 50, 25);
        filterCutoffSliders[i].setBounds(350, y, 120, 25);
        highpassToggleButtons[i].setBounds(480, y, 50, 25);
        highpassCutoffSliders[i].setBounds(540, y, 120, 25);

        y += 30;

        // Linha 2: BPF toggle, BPF cutoff, BPF bandwidth
        bandpassToggleButtons[i].setBounds(10, y, 50, 25);
        bandpassCutoffSliders[i].setBounds(70, y, 120, 25);
        bandpassBandwidthSliders[i].setBounds(200, y, 120, 25);

        y += 40; // espaço após cada animal
    }

    for (int i = 0; i < NUM_BEATS; ++i)
    {
        loadBeatButtons[i].setBounds(10, y, 140, 25);
        playBeatButtons[i].setBounds(160, y, 120, 25);
        y += 35;
    }


    int stepYStart = y + 20;
    int rowHeight = 35;
    int stepSize = 25;
    int buttonSize = 20;
    int stepStartX = 480;

    for (int i = 0; i < NUM_TRACKS; ++i)
    {
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            stepButtons[i][step].setBounds(
                stepStartX + step * stepSize,
                stepYStart + i * rowHeight + 20,
                buttonSize,
                buttonSize
            );
        }
    }

    int groupX = stepStartX - 10;
    int groupY = stepYStart;
    int groupWidth = NUM_STEPS * stepSize + 20;
    int groupHeight = NUM_TRACKS * rowHeight + 30;
    stepSequencerGroup.setBounds(groupX, groupY, groupWidth, groupHeight);

    int labelY = groupY + 10;
    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setBounds(
            stepStartX + step * stepSize,
            labelY,
            30,
            15
        );
    }

    setSize(stepStartX + NUM_STEPS * stepSize + 30, stepYStart + NUM_TRACKS * rowHeight + 60);
}



void AnimalBeatAudioProcessorEditor::timerCallback()
{
    currentStep = audioProcessor.getCurrentStep();
    repaint();
}

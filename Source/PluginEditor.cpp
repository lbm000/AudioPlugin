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
            audioProcessor.isAnimalPlaying[i] = !audioProcessor.isAnimalPlaying[i]; // Toggle manual

            auto isOn = audioProcessor.isAnimalPlaying[i];
            playAnimalButtons[i].setColour(juce::TextButton::buttonColourId, isOn ? juce::Colours::blue : juce::Colours::red);
        };
        playAnimalButtons[i].setColour(juce::TextButton::buttonColourId, juce::Colours::red); // Estado inicial
        addAndMakeVisible(playAnimalButtons[i]);

        animalBpmSliders[i].setRange(1, 300, 1);
        animalBpmSliders[i].setValue(audioProcessor.animalBpms[i]);
        animalBpmSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        animalBpmSliders[i].onValueChange = [this, i]() {
            audioProcessor.setAnimalBpm(i, animalBpmSliders[i].getValue());
        };
        addAndMakeVisible(animalBpmSliders[i]);

        bpmLabels[i].setText("BPM " + juce::String(i + 1), juce::dontSendNotification);
        bpmLabels[i].setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(bpmLabels[i]);
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


        beatBpmSliders[i].setRange(1, 300, 1);
        beatBpmSliders[i].setValue(audioProcessor.beatBpms[i]);
        beatBpmSliders[i].setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
        beatBpmSliders[i].onValueChange = [this, i]() {
            audioProcessor.setBeatBpm(i, beatBpmSliders[i].getValue());
        };
        addAndMakeVisible(beatBpmSliders[i]);

        bpmLabels[labelIndex].setText("BPM " + juce::String(i + 1), juce::dontSendNotification);
        bpmLabels[labelIndex].setJustificationType(juce::Justification::centredLeft);
        addAndMakeVisible(bpmLabels[labelIndex]);
    }

    for (int track = 0; track < NUM_TRACKS; ++track)
    {
        for (int step = 0; step < NUM_STEPS; ++step)
        {
            addAndMakeVisible(stepButtons[track][step]);
        }
    }


    for (int step = 0; step < NUM_STEPS; ++step)
    {
        stepLabels[step].setText(juce::String(step + 1), juce::dontSendNotification);
        stepLabels[step].setJustificationType(juce::Justification::centred);
        stepLabels[step].setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(stepLabels[step]);
    }

    // Inicializa a "caixa" visual do step sequencer
    stepSequencerGroup.setText("Step Sequencer");
    stepSequencerGroup.setColour(juce::GroupComponent::outlineColourId, juce::Colours::grey);
    stepSequencerGroup.setColour(juce::GroupComponent::textColourId, juce::Colours::white);
    addAndMakeVisible(stepSequencerGroup);

    setSize(500, 350);
}


AnimalBeatAudioProcessorEditor::~AnimalBeatAudioProcessorEditor()
{
}

//==============================================================================
void AnimalBeatAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void AnimalBeatAudioProcessorEditor::resized()
{
    int y = 10;

    for (int i = 0; i < NUM_ANIMALS; ++i)
    {
        loadAnimalButtons[i].setBounds(10, y, 140, 25);
        playAnimalButtons[i].setBounds(160, y, 120, 25);
        bpmLabels[i].setBounds(290, y, 40, 25);
        animalBpmSliders[i].setBounds(310, y, 190, 25);
        y += 35;
    }

    for (int i = 0; i < NUM_BEATS; ++i)
    {
        const int labelIndex = NUM_ANIMALS + i;
        loadBeatButtons[i].setBounds(10, y, 140, 25);
        playBeatButtons[i].setBounds(160, y, 120, 25);
        bpmLabels[labelIndex].setBounds(290, y, 40, 25);
        beatBpmSliders[i].setBounds(310, y, 190, 25);
        y += 35;
    }


    int rowY = 10;
    int rowHeight = 35;
    int stepSize = 25;
    int buttonSize = 20;
    int stepStartX = 480;

    for (int i = 0; i < NUM_TRACKS; ++i)
    {
        if (i < NUM_ANIMALS)
        {
            loadAnimalButtons[i].setBounds(10, rowY, 140, 25);
            playAnimalButtons[i].setBounds(160, rowY, 100, 25);
            bpmLabels[i].setBounds(270, rowY, 40, 25);
            animalBpmSliders[i].setBounds(310, rowY, 120, 25);
        }
        else
        {
            int beatIndex = i - NUM_ANIMALS;
            loadBeatButtons[beatIndex].setBounds(10, rowY, 140, 25);
            playBeatButtons[beatIndex].setBounds(160, rowY, 100, 25);
            bpmLabels[i].setBounds(270, rowY, 40, 25);
            beatBpmSliders[beatIndex].setBounds(310, rowY, 120, 25);
        }

        for (int step = 0; step < NUM_STEPS; ++step)
        {
            stepButtons[i][step].setBounds(
                stepStartX + step * stepSize,
                rowY + 20,
                buttonSize,
                buttonSize
            );
        }

        rowY += rowHeight;
    }


    int groupX = stepStartX - 10;
    int groupY = 5;
    int groupWidth = NUM_STEPS * stepSize + 20;
    int groupHeight = rowY + 5;

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


    setSize(stepStartX + NUM_STEPS * stepSize + 20, rowY + 40);



}


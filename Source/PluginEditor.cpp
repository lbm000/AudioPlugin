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

        playAnimalToggles[i].setButtonText("Play Animal " + juce::String(i + 1));
        playAnimalToggles[i].onClick = [this, i]() {
            audioProcessor.isAnimalPlaying[i] = playAnimalToggles[i].getToggleState();
        };
        addAndMakeVisible(playAnimalToggles[i]);

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

        playBeatToggles[i].setButtonText("Play Beat " + juce::String(i + 1));
        playBeatToggles[i].onClick = [this, i]() {
            audioProcessor.isBeatPlaying[i] = playBeatToggles[i].getToggleState();
        };
        addAndMakeVisible(playBeatToggles[i]);

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
        playAnimalToggles[i].setBounds(160, y, 120, 25);
        bpmLabels[i].setBounds(290, y, 40, 25);
        animalBpmSliders[i].setBounds(340, y, 120, 25);
        y += 35;
    }

    for (int i = 0; i < NUM_BEATS; ++i)
    {
        const int labelIndex = NUM_ANIMALS + i;
        loadBeatButtons[i].setBounds(10, y, 140, 25);
        playBeatToggles[i].setBounds(160, y, 120, 25);
        bpmLabels[labelIndex].setBounds(290, y, 40, 25);
        beatBpmSliders[i].setBounds(340, y, 120, 25);
        y += 35;
    }

    setSize(500, y + 10);
}


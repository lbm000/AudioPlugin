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

    loadAnimalButton.setButtonText("Load Animal Sound");
    loadAnimalButton.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select an animal sound", juce::File{}, "*.wav;*.mp3");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                juce::File selectedFile = fc.getResult();
                audioProcessor.loadAnimalFile(selectedFile);
            });
    };
    addAndMakeVisible(loadAnimalButton);


    loadBeatButton.setButtonText("Load Beat");
    loadBeatButton.onClick = [this]()
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select a beat", juce::File{}, "*.wav;*.mp3;*.aiff;*.flac;*.ogg");
        fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                juce::File selectedFile = fc.getResult();
                audioProcessor.loadBeatFile(selectedFile);
            });
    };
    addAndMakeVisible(loadBeatButton);


    addAndMakeVisible(playAnimalToggle);
    playAnimalToggle.setButtonText("Play Animal");

    playAnimalToggle.setToggleState(false, juce::dontSendNotification);
    playAnimalToggle.onClick = [this]() {
        audioProcessor.isAnimalPlaying = playAnimalToggle.getToggleState();
    };


    addAndMakeVisible(playBeatToggle);
    playBeatToggle.setButtonText("Play Beat");

    playBeatToggle.setToggleState(false, juce::dontSendNotification);
    playBeatToggle.onClick = [this]() {
        audioProcessor.isBeatPlaying = playBeatToggle.getToggleState();
    };

    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.setFont(juce::Font(14.0f)); // ✅ forma válida e funcional
    bpmLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(bpmLabel);

    //range of bpm slider
    animalBpmSlider.setRange(1, 60,1.0f);
    // put the value from slider to my bpm variable
    animalBpmSlider.setValue(audioProcessor.animalBpm);

    animalBpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    animalBpmSlider.onValueChange = [this]() {
    	audioProcessor.setAnimalBpm(animalBpmSlider.getValue());
	};
    addAndMakeVisible(animalBpmSlider);

    //range of bpm slider
    beatBpmSlider.setRange(60, 180,1.0f);
    // put the value from slider to my bpm variable
    beatBpmSlider.setValue(audioProcessor.beatBpm);

    beatBpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    beatBpmSlider.onValueChange = [this]() {
    	audioProcessor.setBeatBpm(beatBpmSlider.getValue());
	};
    addAndMakeVisible(beatBpmSlider);


    setSize(500, 400);
}


AnimalBeatAudioProcessorEditor::~AnimalBeatAudioProcessorEditor()
{
}

//==============================================================================
void AnimalBeatAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void AnimalBeatAudioProcessorEditor::resized()
{
    loadAnimalButton.setBounds(10, 10, 180, 30);
    loadBeatButton.setBounds(10, 50, 180, 30);
    playAnimalToggle.setBounds(10, 90, 140, 30);
    playBeatToggle.setBounds(10, 130, 140, 30);
    bpmLabel.setBounds(220, 10, 100, 20);    // x, y, width, height
    animalBpmSlider.setBounds(220, 30, 200, 30);   //
    beatBpmSlider.setBounds(220, 70, 200, 30);



}

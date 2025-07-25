/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AnimalBeatAudioProcessor::AnimalBeatAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    formatManager.registerBasicFormats(); //  .wav, .aiff, .mp3 etc.
}

AnimalBeatAudioProcessor::~AnimalBeatAudioProcessor()
{
}

//==============================================================================
const juce::String AnimalBeatAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AnimalBeatAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AnimalBeatAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AnimalBeatAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AnimalBeatAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AnimalBeatAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AnimalBeatAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AnimalBeatAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AnimalBeatAudioProcessor::getProgramName (int index)
{
    return {};
}

void AnimalBeatAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AnimalBeatAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (sampleRate > 0.0)
        globalSamplesPerBeat = static_cast<int>((60.0 / globalBpm) * sampleRate);

    for (int i = 0; i < NUM_ANIMALS; ++i)
        animalSampleCounters[i] = 0;

    for (int i = 0; i < NUM_BEATS; ++i)
        beatSampleCounters[i] = 0;
}



void AnimalBeatAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AnimalBeatAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AnimalBeatAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int bufferNumSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < bufferNumSamples; ++sample)
    {
        for (int i = 0; i < NUM_ANIMALS; ++i)
        {
            if(animalSampleCounters[i] % globalSamplesPerBeat == 0 &&
                isAnimalFileLoaded[i] && isAnimalPlaying[i])
            {
                animalReadPositions[i] = 0;
            }
        }

        for (int i = 0; i < NUM_BEATS; ++i)
        {
            if (beatSampleCounters[i] % globalSamplesPerBeat == 0 &&
                isBeatFileLoaded[i] && isBeatPlaying[i])
            {
                beatReadPositions[i] = 0;
            }
        }

        for (int channel = 0; channel < numChannels; ++channel)
        {
            float& outSample = buffer.getWritePointer(channel)[sample];

            // Animais
            for (int i = 0; i < NUM_ANIMALS; ++i)
            {
                if (isAnimalFileLoaded[i] && isAnimalPlaying[i] &&
                    animalReadPositions[i] < animalBuffers[i].getNumSamples())
                {
                    float inSample = animalBuffers[i].getReadPointer(channel % animalBuffers[i].getNumChannels())[animalReadPositions[i]];
                    outSample += inSample;
                }
            }

            // Beats
            for (int i = 0; i < NUM_BEATS; ++i)
            {
                if (isBeatFileLoaded[i] && isBeatPlaying[i] &&
                    beatReadPositions[i] < beatBuffers[i].getNumSamples())
                {
                    float inSample = beatBuffers[i].getReadPointer(channel % beatBuffers[i].getNumChannels())[beatReadPositions[i]];
                    outSample += inSample;
                }
            }
        }

        // Avança os ponteiros
        for (int i = 0; i < NUM_ANIMALS; ++i)
        {
            if (isAnimalFileLoaded[i] && isAnimalPlaying[i] &&
                animalReadPositions[i] < animalBuffers[i].getNumSamples())
                ++animalReadPositions[i];

            ++animalSampleCounters[i];
        }

        for (int i = 0; i < NUM_BEATS; ++i)
        {
            if (isBeatFileLoaded[i] && isBeatPlaying[i] &&
                beatReadPositions[i] < beatBuffers[i].getNumSamples())
                ++beatReadPositions[i];

            ++beatSampleCounters[i];
        }
    }
}




//==============================================================================
bool AnimalBeatAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AnimalBeatAudioProcessor::createEditor()
{
    return new AnimalBeatAudioProcessorEditor (*this);
}

//==============================================================================
void AnimalBeatAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AnimalBeatAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AnimalBeatAudioProcessor();
}

void AnimalBeatAudioProcessor::loadAnimalFile(const juce::File& file, int index)
{
    if (index < 0 || index >= NUM_ANIMALS)
        return;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        animalBuffers[index].setSize((int)reader->numChannels, (int)reader->lengthInSamples);
        animalBuffers[index].clear();
        reader->read(&animalBuffers[index], 0, (int)reader->lengthInSamples, 0, true, true);
        animalReadPositions[index] = 0;
        isAnimalFileLoaded[index] = true;
    }
    else
    {
        isAnimalFileLoaded[index] = false;
        DBG("Error loading animal sound into slot " + juce::String(index));
    }
}


void AnimalBeatAudioProcessor::loadBeatFile(const juce::File& file, int index)
{
    if (index < 0 || index >= NUM_BEATS)
        return;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        beatBuffers[index].setSize((int)reader->numChannels, (int)reader->lengthInSamples);
        beatBuffers[index].clear();
        reader->read(&beatBuffers[index], 0, (int)reader->lengthInSamples, 0, true, true);
        beatReadPositions[index] = 0;
        isBeatFileLoaded[index] = true;
    }
    else
    {
        isBeatFileLoaded[index] = false;
        DBG("Error loading beat sound into slot " + juce::String(index));
    }
}


void AnimalBeatAudioProcessor::setGlobalBpm(float newBpm)
{
    globalBpm = newBpm;
    if (getSampleRate() > 0.0)
        globalSamplesPerBeat = static_cast<int>((60.0 / globalBpm) * getSampleRate());
}



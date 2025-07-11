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
    animalSampleCounter = 0;
    beatSampleCounter = 0;

    animalSamplesPerBeat = static_cast<int>((60.0 / animalBpm) * sampleRate);
    beatSamplesPerBeat   = static_cast<int>((60.0 / beatBpm)   * sampleRate);
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
    // Impede problemas com números de ponto flutuante muito pequenos (subnormals)
    juce::ScopedNoDenormals noDenormals;

    // Quantas amostras (samples) existem neste bloco de áudio atual
    const int bufferNumSamples = buffer.getNumSamples();

    // Quantos canais (por ex: 2 = estéreo) o buffer de saída tem
    const int numChannels = buffer.getNumChannels();

    // Itera por cada amostra individual dentro do bloco
    for (int sample = 0; sample < bufferNumSamples; ++sample)
	{
    	// Reiniciar leitura se for momento de "beat" do animal
    	if (animalSampleCounter % animalSamplesPerBeat == 0 &&
        	isAnimalFileLoaded && isAnimalPlaying)
        	animalReadPosition = 0;

    	if (beatSampleCounter % beatSamplesPerBeat == 0 &&
        	isBeatFileLoaded && isBeatPlaying)
        	drumReadPosition = 0;

    for (int channel = 0; channel < numChannels; ++channel)
    {
        float& outSample = buffer.getWritePointer(channel)[sample];

        if (isAnimalFileLoaded && isAnimalPlaying &&
            animalReadPosition < animalBuffer.getNumSamples())
        {
            float inSample = animalBuffer.getReadPointer(channel % animalBuffer.getNumChannels())[animalReadPosition];
            outSample += inSample;
        }

        if (isBeatFileLoaded && isBeatPlaying &&
            drumReadPosition < beatBuffer.getNumSamples())
        {
            float inSample = beatBuffer.getReadPointer(channel % beatBuffer.getNumChannels())[drumReadPosition];
            outSample += inSample;
        }
    }

    if (isAnimalFileLoaded && isAnimalPlaying &&
        animalReadPosition < animalBuffer.getNumSamples())
        ++animalReadPosition;

    if (isBeatFileLoaded && isBeatPlaying &&
        drumReadPosition < beatBuffer.getNumSamples())
        ++drumReadPosition;

    	++animalSampleCounter;
    	++beatSampleCounter;
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

void AnimalBeatAudioProcessor::loadAnimalFile(const juce::File& file)
{
  	// audio reader
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    // reader was created sucessfull ? if yes then go ahead
    if (reader.get() != nullptr)
    {
      	// create the animal buffer with the necessary number of channels and size
        animalBuffer.setSize((int) reader->numChannels, (int) reader->lengthInSamples);

        // takes care that i will not receive trash in my buffer
        animalBuffer.clear();

        // destiny of my audio file is animal buffer
        reader->read(&animalBuffer, 0, (int) reader->lengthInSamples, 0, true, true);
        animalReadPosition = 0;
        isAnimalFileLoaded = true;
    }
    else
    {
        isAnimalFileLoaded = false;
        DBG("Erro ao carregar arquivo de som animal.");
    }
}

void AnimalBeatAudioProcessor::loadBeatFile(const juce::File& file)
{
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader.get() != nullptr)
    {
        beatBuffer.setSize((int) reader->numChannels, (int) reader->lengthInSamples);
        beatBuffer.clear();
        reader->read(&beatBuffer, 0, (int) reader->lengthInSamples, 0, true, true);
        drumReadPosition = 0;
        isBeatFileLoaded = true;
    }
    else
    {
        isBeatFileLoaded = false;
        DBG("Erro ao carregar arquivo de beat.");
    }
}

void AnimalBeatAudioProcessor::setAnimalBpm(float newBpm)
{
    animalBpm = newBpm;

    if (getSampleRate() > 0.0)
        animalSamplesPerBeat = static_cast<int>((60.0 / animalBpm) * getSampleRate());
}

void AnimalBeatAudioProcessor::setBeatBpm(float newBpm)
{
    beatBpm = newBpm;

    if (getSampleRate() > 0.0)
        beatSamplesPerBeat = static_cast<int>((60.0 / beatBpm) * getSampleRate());
}


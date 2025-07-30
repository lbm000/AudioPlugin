#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SampleAudioProcessor::SampleAudioProcessor()
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

SampleAudioProcessor::~SampleAudioProcessor()
{
}

//==============================================================================
const juce::String SampleAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SampleAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SampleAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SampleAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SampleAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SampleAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SampleAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SampleAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SampleAudioProcessor::getProgramName (int index)
{
    return {};
}

void SampleAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SampleAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (sampleRate > 0.0)
        globalSamplesPerBeat = static_cast<int>((60.0 / globalBpm) * sampleRate);

    for (int i = 0; i < NUM_SAMPLES; ++i)
        SampleCounters[i] = 0;


    //  Low-Pass
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        sampleFilters[i].reset();
        sampleFilters[i].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });
        sampleFilters[i].setType(juce::dsp::StateVariableTPTFilterType::lowpass);

        float cutoff = cutoffFrequencies[i] > 0.0f ? cutoffFrequencies[i] : 2000.0f;
        sampleFilters[i].setCutoffFrequency(cutoff);
    }

    // High-Pass
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        sampleHighPassFilters[i].reset();
        sampleHighPassFilters[i].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });
        sampleHighPassFilters[i].setType(juce::dsp::StateVariableTPTFilterType::highpass);

        float hpfCutoff = highPassCutoffFrequencies[i] > 0.0f ? highPassCutoffFrequencies[i] : 1000.0f;
        sampleHighPassFilters[i].setCutoffFrequency(hpfCutoff);
    }


    // Band-pass filter
    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        sampleBandPassFilters[i].reset();
        sampleBandPassFilters[i].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });
        sampleBandPassFilters[i].setType(juce::dsp::StateVariableTPTFilterType::bandpass);

        float cutoff = bandPassCutoffs[i] > 0.0f ? bandPassCutoffs[i] : 1000.0f;
        float bandwidth = bandPassBandwidths[i] > 0.0f ? bandPassBandwidths[i] : 500.0f;
        float q = cutoff / bandwidth;

        sampleBandPassFilters[i].setCutoffFrequency(cutoff);
        sampleBandPassFilters[i].setResonance(q);
    }

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        sampleNotchFilters[i].reset();
        sampleNotchFilters[i].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });

        updateNotchCoefficients(i);
    }


    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        samplePeakFilters[i].reset();
        samplePeakFilters[i].prepare({ sampleRate, static_cast<juce::uint32>(samplesPerBlock), 1 });

        float cutoff = peakCutoffs[i] > 0.0f ? peakCutoffs[i] : 1000.0f;
        float gain = peakGains[i]; //
        float q = peakQs[i] > 0.0f ? peakQs[i] : 1.0f;

        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(sampleRate, cutoff, q, juce::Decibels::decibelsToGain(gain));
        *samplePeakFilters[i].coefficients = *coeffs;
    }

    for (int i = 0; i < NUM_SAMPLES; ++i)
    {
        adsrEnvelopes[i].setSampleRate(sampleRate);


        adsrParams[i].attack  = 0.01f;
        adsrParams[i].decay   = 0.1f;
        adsrParams[i].sustain = 1.0f;
        adsrParams[i].release = 0.1f;

        adsrEnvelopes[i].setParameters(adsrParams[i]);
    }

}





void SampleAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SampleAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void SampleAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int bufferNumSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    for (int sample = 0; sample < bufferNumSamples; ++sample)
    {

        for (int i = 0; i < NUM_SAMPLES; ++i)
        {
            if (isSampleFileLoaded[i] && isSamplePlaying[i] && stepStates[i][currentStep])
            {
                if (sampleReadPositions[i] == 0)
                    adsrEnvelopes[i].noteOn();
                else if (sampleReadPositions[i] >= sampleBuffers[i].getNumSamples())
                    adsrEnvelopes[i].noteOff();
            }
        }


        for (int channel = 0; channel < numChannels; ++channel)
        {
            float& outSample = buffer.getWritePointer(channel)[sample];
            outSample = 0.0f;


            for (int i = 0; i < NUM_SAMPLES; ++i)
            {
                if (isSampleFileLoaded[i] && isSamplePlaying[i] &&
                    stepStates[i][currentStep] &&
                    sampleReadPositions[i] < sampleBuffers[i].getNumSamples())
                {
                    float inSample = sampleBuffers[i]
                        .getReadPointer(channel % sampleBuffers[i].getNumChannels())[sampleReadPositions[i]];

                    if (isNotchEnabled[i])
                    {
                        inSample = sampleNotchFilters[i].processSample(inSample);
                    }

                    else if (isBandPassEnabled[i])
                    {
                        float cutoff = bandPassCutoffs[i] > 0.0f ? bandPassCutoffs[i] : 1000.0f;
                        float bandwidth = bandPassBandwidths[i] > 1.0f ? bandPassBandwidths[i] : 1.0f;
                        float q = cutoff / bandwidth;

                        sampleBandPassFilters[i].setCutoffFrequency(cutoff);
                        sampleBandPassFilters[i].setResonance(q);

                        inSample = sampleBandPassFilters[i].processSample(0, inSample);
                    }

                    else if (isPeakEnabled[i])
                    {
                        float cutoff = peakCutoffs[i] > 0.0f ? peakCutoffs[i] : 1000.0f;
                        float gain = peakGains[i];
                        float q = peakQs[i] > 0.0f ? peakQs[i] : 1.0f;

                        auto coeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(getSampleRate(), cutoff, q, juce::Decibels::decibelsToGain(gain));
                        *samplePeakFilters[i].coefficients = *coeffs;

                        inSample = samplePeakFilters[i].processSample(inSample);
                    }
                    else
                    {
                        if (isHighPassEnabled[i])
                            inSample = sampleHighPassFilters[i].processSample(0, inSample);

                        if (isFilterEnabled[i])
                            inSample = sampleFilters[i].processSample(0, inSample);
                    }

                    if (isBitcrusherEnabled[i])
                    {
                        int& counter = downsampleCounters[i];
                        int downsampleFactor = std::max(1, static_cast<int>(downsampleRates[i]));

                        if (counter == 0)
                        {
                            int bitDepth = std::clamp(bitDepths[i], 1, 24);
                            float maxVal = static_cast<float>((1 << bitDepth) - 1);
                            inSample = std::round(inSample * maxVal) / maxVal;
                        }

                        counter = (counter + 1) % downsampleFactor;
                    }

                    // ADSR and gain
                    float envelopeValue = adsrEnvelopes[i].getNextSample();
                    inSample *= envelopeValue * gainLevels[i];
                    outSample += inSample;
                }
            }
        }


        for (int i = 0; i < NUM_SAMPLES; ++i)
        {
            if (isSampleFileLoaded[i] && isSamplePlaying[i] &&
                stepStates[i][currentStep] &&
                sampleReadPositions[i] < sampleBuffers[i].getNumSamples())
            {
                ++sampleReadPositions[i];
            }
        }


    }

    int samplesPerStep = globalSamplesPerBeat / 4;
    sampleCounterForStep += bufferNumSamples;

    sampleCounterForStep += bufferNumSamples;

    while (sampleCounterForStep >= samplesPerStep)
    {
        sampleCounterForStep -= samplesPerStep;
        currentStep = (currentStep + 1) % NUM_STEPS;

        for (int i = 0; i < NUM_SAMPLES; ++i)
        {
            if (stepStates[i][currentStep])
                sampleReadPositions[i] = 0;
        }
    }

}


//==============================================================================
bool SampleAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SampleAudioProcessor::createEditor()
{
    return new SampleAudioProcessorEditor (*this);
}

//==============================================================================
void SampleAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SampleAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SampleAudioProcessor();
}

void SampleAudioProcessor::loadSampleFile(const juce::File& file, int index)
{
    if (index < 0 || index >= NUM_SAMPLES)
        return;

    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));
    if (reader)
    {
        sampleBuffers[index].setSize((int)reader->numChannels, (int)reader->lengthInSamples);
        sampleBuffers[index].clear();
        reader->read(&sampleBuffers[index], 0, (int)reader->lengthInSamples, 0, true, true);
        sampleReadPositions[index] = 0;
        isSampleFileLoaded[index] = true;
    }
    else
    {
        isSampleFileLoaded[index] = false;
        DBG("Error loading sample sound into slot " + juce::String(index));
    }
}





void SampleAudioProcessor::setGlobalBpm(float newBpm)
{
    globalBpm = newBpm;
    if (getSampleRate() > 0.0)
        globalSamplesPerBeat = static_cast<int>((60.0 / globalBpm) * getSampleRate());
}

void SampleAudioProcessor::setFilterEnabled(int index, bool enabled)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        isFilterEnabled[index] = enabled;

        if (enabled)
            isBandPassEnabled[index] = false;
    }
}

void SampleAudioProcessor::setFilterCutoff(int index, float cutoffHz)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        cutoffFrequencies[index] = cutoffHz;


        sampleFilters[index].setCutoffFrequency(cutoffHz);
    }
}

bool SampleAudioProcessor::getHighpassEnabled(int index) const
{
    return isHighPassEnabled[index];
}

void SampleAudioProcessor::setHighpassEnabled(int index, bool enabled)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        isHighPassEnabled[index] = enabled;

        if (enabled)
            isBandPassEnabled[index] = false;
    }
}

float SampleAudioProcessor::getHighpassCutoff(int index) const
{
    return highPassCutoffFrequencies[index];
}

void SampleAudioProcessor::setHighpassCutoff(int index, float cutoff)
{
    highPassCutoffFrequencies[index] = cutoff;
    sampleHighPassFilters[index].setCutoffFrequency(cutoff);
}

bool SampleAudioProcessor::getBandPassEnabled(int index) const
{
    return isBandPassEnabled[index];
}

void SampleAudioProcessor::setBandPassEnabled(int index, bool enabled)
{
    isBandPassEnabled[index] = enabled;

    if (enabled)
    {

        isFilterEnabled[index] = false;
        isHighPassEnabled[index] = false;
    }
}

void SampleAudioProcessor::setBandPassCutoff(int index, float value) {
    if (index >= 0 && index < NUM_SAMPLES)
        bandPassCutoffs[index] = value;
}

float SampleAudioProcessor::getBandPassCutoff(int index) const {
    return bandPassCutoffs[index];
}

void SampleAudioProcessor::setBandPassBandwidth(int index, float value) {
    if (index >= 0 && index < NUM_SAMPLES)
        bandPassBandwidths[index] = value;
}

float SampleAudioProcessor::getBandPassBandwidth(int index) const {
    return bandPassBandwidths[index];
}

bool SampleAudioProcessor::getNotchEnabled(int index) const
{
    return isNotchEnabled[index];
}

void SampleAudioProcessor::setNotchEnabled(int index, bool enabled)
{
    isNotchEnabled[index] = enabled;

    if (enabled)
    {
        isFilterEnabled[index] = false;
        isHighPassEnabled[index] = false;
        isBandPassEnabled[index] = false;
    }
}

void SampleAudioProcessor::setNotchCutoff(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        notchCutoffs[index] = value;
        updateNotchCoefficients(index);
    }
}


void SampleAudioProcessor::setNotchBandwidth(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        notchBandwidths[index] = value;
        updateNotchCoefficients(index);
    }
}


void SampleAudioProcessor::updateNotchCoefficients(int index)
{
    if (index < 0 || index >= NUM_SAMPLES) return;

    float cutoff = notchCutoffs[index] > 0.0f ? notchCutoffs[index] : 1000.0f;
    float bandwidth = notchBandwidths[index] > 1.0f ? notchBandwidths[index] : 100.0f;
    float q = cutoff / bandwidth;

    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeNotch(getSampleRate(), cutoff, q);
    *sampleNotchFilters[index].coefficients = *coeffs;
}

bool SampleAudioProcessor::getPeakEnabled(int index) const
{
    return isPeakEnabled[index];
}

void SampleAudioProcessor::setPeakEnabled(int index, bool enabled)
{
    isPeakEnabled[index] = enabled;

    if (enabled)
    {
        isBandPassEnabled[index] = false;
    }
}

void SampleAudioProcessor::setPeakCutoff(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
        peakCutoffs[index] = value;
}

void SampleAudioProcessor::setPeakGain(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
        peakGains[index] = value;
}

void SampleAudioProcessor::setPeakQ(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
        peakQs[index] = value;
}


void SampleAudioProcessor::setBitcrusherEnabled(int index, bool enabled)
{
    isBitcrusherEnabled[index] = enabled;
}

void SampleAudioProcessor::setBitDepth(int index, int depth)
{
    bitDepths[index] = depth;
}

void SampleAudioProcessor::setDownsampleRate(int index, float rate)
{
    downsampleRates[index] = rate;
}

void SampleAudioProcessor::setGainLevel(int index, float gain)
{
    if (index >= 0 && index < NUM_SAMPLES)
        gainLevels[index] = gain;
}

float SampleAudioProcessor::getGainLevel(int index) const
{
    return (index >= 0 && index < NUM_SAMPLES) ? gainLevels[index] : 1.0f;
}

void SampleAudioProcessor::setAdsrAttack(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        adsrParams[index].attack = value;
        adsrEnvelopes[index].setParameters(adsrParams[index]);
    }
}

void SampleAudioProcessor::setAdsrDecay(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        adsrParams[index].decay = value;
        adsrEnvelopes[index].setParameters(adsrParams[index]);
    }
}

void SampleAudioProcessor::setAdsrSustain(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        adsrParams[index].sustain = value;
        adsrEnvelopes[index].setParameters(adsrParams[index]);
    }
}

void SampleAudioProcessor::setAdsrRelease(int index, float value)
{
    if (index >= 0 && index < NUM_SAMPLES)
    {
        adsrParams[index].release = value;
        adsrEnvelopes[index].setParameters(adsrParams[index]);
    }
}

float SampleAudioProcessor::getAdsrAttack(int index) const
{
    return (index >= 0 && index < NUM_SAMPLES) ? adsrParams[index].attack : 0.0f;
}

float SampleAudioProcessor::getAdsrDecay(int index) const
{
    return (index >= 0 && index < NUM_SAMPLES) ? adsrParams[index].decay : 0.0f;
}

float SampleAudioProcessor::getAdsrSustain(int index) const
{
    return (index >= 0 && index < NUM_SAMPLES) ? adsrParams[index].sustain : 0.0f;
}

float SampleAudioProcessor::getAdsrRelease(int index) const
{
    return (index >= 0 && index < NUM_SAMPLES) ? adsrParams[index].release : 0.0f;
}









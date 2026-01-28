#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
StemSplitterSamplerAudioProcessor::StemSplitterSamplerAudioProcessor()
     : juce::AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    // Initialize parameters
    addParameter(drumLevel = new juce::AudioParameterFloat("drumLevel", "Drum Level", 0.0f, 1.0f, 0.8f));
    addParameter(bassLevel = new juce::AudioParameterFloat("bassLevel", "Bass Level", 0.0f, 1.0f, 0.8f));
    addParameter(otherLevel = new juce::AudioParameterFloat("otherLevel", "Other Level", 0.0f, 1.0f, 0.8f));
    addParameter(vocalLevel = new juce::AudioParameterFloat("vocalLevel", "Vocal Level", 0.0f, 1.0f, 0.8f));
    addParameter(separationQuality = new juce::AudioParameterFloat("quality", "Quality", 0.0f, 3.0f, 2.0f));
    addParameter(outputMode = new juce::AudioParameterFloat("outputMode", "Output Mode", 0.0f, 1.0f, 0.0f));
}

StemSplitterSamplerAudioProcessor::~StemSplitterSamplerAudioProcessor()
{
}

//==============================================================================
const juce::String StemSplitterSamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool StemSplitterSamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool StemSplitterSamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool StemSplitterSamplerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double StemSplitterSamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int StemSplitterSamplerAudioProcessor::getNumPrograms()
{
    return 1;
}

int StemSplitterSamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void StemSplitterSamplerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String StemSplitterSamplerAudioProcessor::getProgramName (int index)
{
    return {};
}

void StemSplitterSamplerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void StemSplitterSamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBufferSize = samplesPerBlock;
    
    // Initialize stem separator
    stemSeparator = std::make_unique<StemSeparator>();
    stemSeparator->initialize(sampleRate, samplesPerBlock);
    
    // Initialize sampler
    sampler = std::make_unique<SamplerComponent>();
    sampler->initialize(sampleRate, samplesPerBlock);
    
    // Initialize stem buffers
    for (auto& buffer : stemBuffers)
    {
        buffer.setSize(2, samplesPerBlock);
    }
}

void StemSplitterSamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool StemSplitterSamplerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // Support stereo input/output
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono())
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

void StemSplitterSamplerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, 
                                               juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that don't contain input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (stemSeparator && sampler)
    {
        // Separate stems from input
        stemSeparator->setModelQuality(static_cast<int>(*separationQuality));
        stemSeparator->processBlock(buffer, stemBuffers);
        
        // Load stems into sampler (only do this once when input changes)
        if (!samplesLoaded)
        {
            for (int i = 0; i < 4; ++i)
            {
                sampler->loadStem(i, stemBuffers[i], currentSampleRate);
            }
            samplesLoaded = true;
        }
        
        // Process MIDI and generate output from sampler
        sampler->processBlock(buffer);
        
        // Apply stem level controls
        if (static_cast<int>(*outputMode) == 0) // All stems mode
        {
            for (int channel = 0; channel < totalNumOutputChannels; ++channel)
            {
                auto* channelData = buffer.getWritePointer(channel);
                
                for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
                {
                    // Apply stem-specific level controls (simplified)
                    channelData[sample] *= (*drumLevel + *bassLevel + *otherLevel + *vocalLevel) * 0.25f;
                }
            }
        }
    }
    else
    {
        // Fallback: pass audio through
        // This shouldn't happen if initialization succeeded
    }
}

//==============================================================================
bool StemSplitterSamplerAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* StemSplitterSamplerAudioProcessor::createEditor()
{
    return new StemSplitterSamplerAudioProcessorEditor (*this);
}

//==============================================================================
void StemSplitterSamplerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // Save plugin state
    juce::MemoryOutputStream stream(destData, true);
    stream.writeFloat(*drumLevel);
    stream.writeFloat(*bassLevel);
    stream.writeFloat(*otherLevel);
    stream.writeFloat(*vocalLevel);
    stream.writeFloat(*separationQuality);
    stream.writeFloat(*outputMode);
}

void StemSplitterSamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // Restore plugin state
    juce::MemoryInputStream stream(data, static_cast<size_t>(sizeInBytes), false);
    *drumLevel = stream.readFloat();
    *bassLevel = stream.readFloat();
    *otherLevel = stream.readFloat();
    *vocalLevel = stream.readFloat();
    *separationQuality = stream.readFloat();
    *outputMode = stream.readFloat();
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new StemSplitterSamplerAudioProcessor();
}
#pragma once

#include <JuceHeader.h>
#include "StemSeparator.h"
#include "SamplerComponent.h"

class StemSplitterSamplerAudioProcessor  : public juce::AudioProcessor
{
public:
    StemSplitterSamplerAudioProcessor();
    ~StemSplitterSamplerAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    const juce::String getInputChannelName (int channelIndex) const override;
    const juce::String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;

    bool acceptsInput() const override;
    bool producesOutput() const override;
    bool silenceInProducesSilenceOut() const override;
    double getLatencySamples() const override;

    bool hasEditor() const override;
    juce::AudioProcessorEditor* createEditor() override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    // Stem-specific parameters
    std::atomic<float>* drumLevel;
    std::atomic<float>* bassLevel;
    std::atomic<float>* otherLevel;
    std::atomic<float>* vocalLevel;
    std::atomic<float>* separationQuality;
    std::atomic<float>* outputMode; // 0=All stems, 1=Selected stem only

private:
    std::unique_ptr<StemSeparator> stemSeparator;
    std::unique_ptr<SamplerComponent> sampler;
    std::array<juce::AudioBuffer<float>, 4> stemBuffers;
    
    bool samplesLoaded = false;
    double currentSampleRate = 44100.0;
    int currentBufferSize = 512;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StemSplitterSamplerAudioProcessor)
};
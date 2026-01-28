#include "SamplerComponent.h"

SamplerComponent::SamplerComponent()
{
    for (auto& voice : voices)
    {
        voice.sampleIndex = -1;
        voice.position = 0.0;
        voice.velocity = 0.0f;
        voice.isActive = false;
        voice.currentPitch = 1.0f;
    }
    
    for (int i = 0; i < 4; ++i)
    {
        filterFreqSmooth[i].reset(currentSampleRate, 0.01);
        filterResSmooth[i].reset(currentSampleRate, 0.01);
    }
}

SamplerComponent::~SamplerComponent()
{
}

void SamplerComponent::initialize(int sampleRate, int bufferSize)
{
    currentSampleRate = sampleRate;
    this->bufferSize = bufferSize;
    
    for (int i = 0; i < 4; ++i)
    {
        filterFreqSmooth[i].reset(sampleRate, 0.01);
        filterResSmooth[i].reset(sampleRate, 0.01);
        filterFreqSmooth[i].setCurrentAndTargetValue(20000.0f);
        filterResSmooth[i].setCurrentAndTargetValue(0.1f);
    }
}

void SamplerComponent::loadStem(int stemIndex, const juce::AudioBuffer<float>& stemData, double sampleRate)
{
    if (stemIndex < 0 || stemIndex >= 4)
        return;
    
    auto& sample = stemSamples[stemIndex];
    sample.audioData.makeCopyOf(stemData);
    sample.sourceSampleRate = sampleRate;
    sample.endSeconds = stemData.getNumSamples() / sampleRate;
    sample.isLoaded = true;
    
    juce::Logger::writeToLog("Loaded stem " + juce::String(stemIndex) + 
                            " with " + juce::String(stemData.getNumSamples()) + " samples");
}

void SamplerComponent::noteOn(int midiNote, float velocity)
{
    if (velocity <= 0.0f)
        return;
    
    // Find free voice
    for (auto& voice : voices)
    {
        if (!voice.isActive)
        {
            voice.sampleIndex = midiNote % 4; // Map MIDI note to stem
            voice.position = 0.0;
            voice.velocity = velocity;
            voice.isActive = true;
            voice.currentPitch = midiNoteToFrequency(midiNote) / midiNoteToFrequency(60);
            break;
        }
    }
}

void SamplerComponent::noteOff(int midiNote)
{
    // Stop voices with matching MIDI note
    for (auto& voice : voices)
    {
        if (voice.isActive && voice.sampleIndex == midiNote % 4)
        {
            voice.isActive = false;
            voice.velocity = 0.0f;
        }
    }
}

void SamplerComponent::allNotesOff()
{
    for (auto& voice : voices)
    {
        voice.isActive = false;
        voice.velocity = 0.0f;
    }
}

void SamplerComponent::processBlock(juce::AudioBuffer<float>& outputBuffer)
{
    outputBuffer.clear();
    
    for (auto& voice : voices)
    {
        if (!voice.isActive || voice.sampleIndex < 0 || voice.sampleIndex >= 4)
            continue;
        
        auto& sample = stemSamples[voice.sampleIndex];
        if (!sample.isLoaded)
            continue;
        
        const int numSamples = outputBuffer.getNumSamples();
        const int numChannels = outputBuffer.getNumChannels();
        
        // Calculate playback parameters
        const double sampleRateRatio = currentSampleRate / sample.sourceSampleRate;
        const double pitchModifiedRate = sampleRateRatio * voice.currentPitch * sample.pitchRatio;
        
        const int startSample = static_cast<int>(sample.startSeconds * sample.sourceSampleRate);
        const int endSample = static_cast<int>(sample.endSeconds * sample.sourceSampleRate);
        const int totalSamples = endSample - startSample;
        
        // Generate sample data
        juce::AudioBuffer<float> voiceBuffer(numChannels, numSamples);
        voiceBuffer.clear();
        
        for (int sampleIdx = 0; sampleIdx < numSamples; ++sampleIdx)
        {
            const int sourceSample = static_cast<int>(voice.position) + startSample;
            
            if (sourceSample >= endSample)
            {
                if (sample.loopEnabled && totalSamples > 0)
                {
                    voice.position = fmod(voice.position, totalSamples);
                }
                else
                {
                    voice.isActive = false;
                    break;
                }
            }
            
            if (sourceSample >= 0 && sourceSample < sample.audioData.getNumSamples())
            {
                for (int ch = 0; ch < numChannels; ++ch)
                {
                    const int sourceChannel = juce::jmin(ch, sample.audioData.getNumChannels() - 1);
                    float sampleValue = sample.audioData.getSample(sourceChannel, sourceSample);
                    
                    // Apply velocity and basic envelope
                    sampleValue *= voice.velocity;
                    
                    voiceBuffer.setSample(ch, sampleIdx, sampleValue);
                }
            }
            
            voice.position += pitchModifiedRate;
        }
        
        // Apply filter and add to output
        applyFilter(voiceBuffer, voice.sampleIndex);
        
        for (int ch = 0; ch < numChannels; ++ch)
        {
            outputBuffer.addFrom(ch, 0, voiceBuffer, ch, 0, numSamples);
        }
    }
}

void SamplerComponent::applyFilter(juce::AudioBuffer<float>& buffer, int sampleIndex)
{
    if (sampleIndex < 0 || sampleIndex >= 4)
        return;
    
    auto& sample = stemSamples[sampleIndex];
    
    // Update smoothed values
    filterFreqSmooth[sampleIndex].setTargetValue(sample.filterFreq);
    filterResSmooth[sampleIndex].setTargetValue(sample.filterRes);
    
    // Apply simple low-pass filter
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        auto* channelData = buffer.getWritePointer(ch);
        
        for (int n = 0; n < numSamples; ++n)
        {
            const float freq = filterFreqSmooth[sampleIndex].getNextValue();
            const float res = filterResSmooth[sampleIndex].getNextValue();
            
            // Simple one-pole low-pass filter (for demo purposes)
            if (freq < 20000.0f)
            {
                const float cutoff = juce::jmap(freq, 20.0f, 20000.0f, 0.0f, 1.0f);
                const float alpha = juce::jlimit(0.0f, 0.99f, 1.0f - cutoff);
                
                if (n > 0)
                {
                    channelData[n] = alpha * channelData[n-1] + (1.0f - alpha) * channelData[n];
                }
            }
        }
    }
}

float SamplerComponent::midiNoteToFrequency(int midiNote) const
{
    return 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);
}

void SamplerComponent::setSampleStart(int stemIndex, double startSeconds)
{
    if (stemIndex >= 0 && stemIndex < 4)
    {
        stemSamples[stemIndex].startSeconds = juce::jmax(0.0, startSeconds);
    }
}

void SamplerComponent::setSampleEnd(int stemIndex, double endSeconds)
{
    if (stemIndex >= 0 && stemIndex < 4)
    {
        stemSamples[stemIndex].endSeconds = endSeconds;
    }
}

void SamplerComponent::setLoopEnabled(int stemIndex, bool shouldLoop)
{
    if (stemIndex >= 0 && stemIndex < 4)
    {
        stemSamples[stemIndex].loopEnabled = shouldLoop;
    }
}

void SamplerComponent::setPitch(int stemIndex, float pitchRatio)
{
    if (stemIndex >= 0 && stemIndex < 4)
    {
        stemSamples[stemIndex].pitchRatio = juce::jmax(0.1f, pitchRatio);
    }
}

void SamplerComponent::setFilter(int stemIndex, float frequency, float resonance)
{
    if (stemIndex >= 0 && stemIndex < 4)
    {
        stemSamples[stemIndex].filterFreq = juce::jlimit(20.0f, 20000.0f, frequency);
        stemSamples[stemIndex].filterRes = juce::jlimit(0.1f, 10.0f, resonance);
    }
}

bool SamplerComponent::isSampleLoaded(int stemIndex) const
{
    return (stemIndex >= 0 && stemIndex < 4) ? stemSamples[stemIndex].isLoaded : false;
}

double SamplerComponent::getSampleLength(int stemIndex) const
{
    if (stemIndex >= 0 && stemIndex < 4)
    {
        const auto& sample = stemSamples[stemIndex];
        return sample.endSeconds - sample.startSeconds;
    }
    return 0.0;
}
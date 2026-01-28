#pragma once

#include <JuceHeader.h>

class StemSeparator
{
public:
    enum class StemType
    {
        Drums,
        Bass,
        Other,
        Vocals,
        Total
    };

    StemSeparator();
    ~StemSeparator();

    void initialize(int sampleRate, int bufferSize);
    void processBlock(juce::AudioBuffer<float>& inputBuffer,
                     std::array<juce::AudioBuffer<float>, 4>& stemOutputs);
    
    bool isInitialized() const { return initialized; }
    
    void setModelQuality(int quality); // 0-3 for different Demucs models
    
private:
    void loadDemucsModel();
    void processWithDemucs(const float* input, float** outputs, int numSamples);
    
    bool initialized = false;
    int currentSampleRate = 44100;
    int currentBufferSize = 512;
    int modelQuality = 2;
    
    // Demucs model interface
    void* demucsModel = nullptr;
    juce::AudioBuffer<float> processingBuffer;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StemSeparator)
};
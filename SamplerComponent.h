#pragma once

#include <JuceHeader.h>

class SamplerComponent
{
public:
    SamplerComponent();
    ~SamplerComponent();

    void initialize(int sampleRate, int bufferSize);
    
    // Load audio into sampler (from stem separation results)
    void loadStem(int stemIndex, const juce::AudioBuffer<float>& stemData, double sampleRate);
    
    // Playback control
    void noteOn(int midiNote, float velocity);
    void noteOff(int midiNote);
    void allNotesOff();
    
    // Process audio
    void processBlock(juce::AudioBuffer<float>& outputBuffer);
    
    // Sampler parameters
    void setSampleStart(int stemIndex, double startSeconds);
    void setSampleEnd(int stemIndex, double endSeconds);
    void setLoopEnabled(int stemIndex, bool shouldLoop);
    void setPitch(int stemIndex, float pitchRatio);
    void setFilter(int stemIndex, float frequency, float resonance);
    
    // Get current loaded sample info
    bool isSampleLoaded(int stemIndex) const;
    double getSampleLength(int stemIndex) const;
    
private:
    struct SampleData
    {
        juce::AudioBuffer<float> audioData;
        double sourceSampleRate = 44100.0;
        double startSeconds = 0.0;
        double endSeconds = 0.0;
        bool loopEnabled = false;
        float pitchRatio = 1.0f;
        bool isLoaded = false;
        
        // Filter parameters
        float filterFreq = 20000.0f;
        float filterRes = 0.1f;
    };
    
    struct Voice
    {
        int sampleIndex = -1;
        double position = 0.0;
        float velocity = 0.0f;
        bool isActive = false;
        float currentPitch = 1.0f;
    };
    
    void applyFilter(juce::AudioBuffer<float>& buffer, int sampleIndex);
    float midiNoteToFrequency(int midiNote) const;
    
    std::array<SampleData, 4> stemSamples;
    std::array<Voice, 16> voices; // Polyphony limit
    int currentSampleRate = 44100;
    int bufferSize = 512;
    
    juce::SmoothedValue<float> filterFreqSmooth[4];
    juce::SmoothedValue<float> filterResSmooth[4];
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SamplerComponent)
};
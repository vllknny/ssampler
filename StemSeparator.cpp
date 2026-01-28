#include "StemSeparator.h"
#include "DemucsInterface.h"

StemSeparator::StemSeparator()
{
    processingBuffer.setSize(2, 8192);
}

StemSeparator::~StemSeparator()
{
    if (demucsModel)
    {
        demucs_cleanup(demucsModel);
    }
}

void StemSeparator::initialize(int sampleRate, int bufferSize)
{
    currentSampleRate = sampleRate;
    currentBufferSize = bufferSize;
    
    // Initialize Demucs model
    loadDemucsModel();
    
    initialized = true;
}

void StemSeparator::loadDemucsModel()
{
    // Clean up existing model
    if (demucsModel)
    {
        demucs_cleanup(demucsModel);
        demucsModel = nullptr;
    }
    
    // Load Demucs model based on quality setting
    const char* modelPath = nullptr;
    
    switch (modelQuality)
    {
        case 0: modelPath = "models/demucs_light.th"; break;
        case 1: modelPath = "models/demucs.th"; break;
        case 2: modelPath = "models/htdemucs.th"; break;
        case 3: modelPath = "models/htdemucs_6s.th"; break;
        default: modelPath = "models/htdemucs.th"; break;
    }
    
    demucsModel = demucs_load_model(modelPath, currentSampleRate);
    
    if (!demucsModel)
    {
        // Fallback to basic separation if model fails to load
        demucsModel = demucs_load_model(nullptr, currentSampleRate);
    }
}

void StemSeparator::processBlock(juce::AudioBuffer<float>& inputBuffer,
                                std::array<juce::AudioBuffer<float>, 4>& stemOutputs)
{
    if (!initialized || !demucsModel)
    {
        // Pass through to all stems if not initialized
        for (auto& stem : stemOutputs)
        {
            stem.setSize(inputBuffer.getNumChannels(), inputBuffer.getNumSamples());
            stem.makeCopyOf(inputBuffer);
        }
        return;
    }
    
    const int numSamples = inputBuffer.getNumSamples();
    const int numChannels = inputBuffer.getNumChannels();
    
    // Ensure output buffers are properly sized
    for (auto& stem : stemOutputs)
    {
        stem.setSize(2, numSamples, false, false, false);
        stem.clear();
    }
    
    // Process in chunks if buffer is too large
    const int chunkSize = 8192;
    for (int start = 0; start < numSamples; start += chunkSize)
    {
        int chunkLength = juce::jmin(chunkSize, numSamples - start);
        
        // Copy input chunk to processing buffer
        processingBuffer.setSize(numChannels, chunkLength, false, false, false);
        for (int ch = 0; ch < numChannels; ++ch)
        {
            processingBuffer.copyFrom(ch, 0, inputBuffer, ch, start, chunkLength);
        }
        
        // Process with Demucs
        float* stems[4];
        for (int i = 0; i < 4; ++i)
        {
            stems[i] = stemOutputs[i].getWritePointer(0, start);
        }
        
        processWithDemucs(processingBuffer.getReadPointer(0), stems, chunkLength);
        
        // Copy right channel if stereo
        if (numChannels > 1)
        {
            for (int i = 0; i < 4; ++i)
            {
                stemOutputs[i].copyFrom(1, start, stemOutputs[i], 0, start, chunkLength);
            }
        }
    }
}

void StemSeparator::processWithDemucs(const float* input, float** outputs, int numSamples)
{
    if (!demucsModel)
        return;
        
    // Convert to Demucs format and process
    // This is a simplified interface - actual implementation would need
    // proper format conversion and tensor handling
    demucs_separate(demucsModel, input, outputs, numSamples);
}

void StemSeparator::setModelQuality(int quality)
{
    if (modelQuality != quality)
    {
        modelQuality = quality;
        if (initialized)
        {
            loadDemucsModel();
        }
    }
}
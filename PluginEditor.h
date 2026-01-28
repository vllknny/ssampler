#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class StemSplitterSamplerAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    StemSplitterSamplerAudioProcessorEditor (StemSplitterSamplerAudioProcessor&);
    ~StemSplitterSamplerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    StemSplitterSamplerAudioProcessor& audioProcessor;
    
    // GUI components
    juce::Slider drumSlider;
    juce::Slider bassSlider;
    juce::Slider otherSlider;
    juce::Slider vocalSlider;
    juce::Slider qualitySlider;
    juce::ComboBox outputModeCombo;
    
    // Labels
    juce::Label drumLabel;
    juce::Label bassLabel;
    juce::Label otherLabel;
    juce::Label vocalLabel;
    juce::Label qualityLabel;
    juce::Label outputModeLabel;
    
    // Attachments for parameters
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> drumAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bassAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> otherAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> vocalAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> qualityAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> outputModeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (StemSplitterSamplerAudioProcessorEditor)
};
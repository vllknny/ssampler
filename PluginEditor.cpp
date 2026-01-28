#include "PluginEditor.h"

//==============================================================================
StemSplitterSamplerAudioProcessorEditor::StemSplitterSamplerAudioProcessorEditor (StemSplitterSamplerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Set up sliders
    drumSlider.setSliderStyle(juce::Slider::LinearVertical);
    drumSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    drumSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(drumSlider);
    
    bassSlider.setSliderStyle(juce::Slider::LinearVertical);
    bassSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    bassSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(bassSlider);
    
    otherSlider.setSliderStyle(juce::Slider::LinearVertical);
    otherSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    otherSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(otherSlider);
    
    vocalSlider.setSliderStyle(juce::Slider::LinearVertical);
    vocalSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    vocalSlider.setRange(0.0, 1.0, 0.01);
    addAndMakeVisible(vocalSlider);
    
    qualitySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    qualitySlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    qualitySlider.setRange(0.0, 3.0, 1.0);
    addAndMakeVisible(qualitySlider);
    
    // Set up combo box
    outputModeCombo.addItem("All Stems", 1);
    outputModeCombo.addItem("Selected Stem", 2);
    addAndMakeVisible(outputModeCombo);
    
    // Set up labels
    drumLabel.setText("Drums", juce::dontSendNotification);
    drumLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    drumLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(drumLabel);
    
    bassLabel.setText("Bass", juce::dontSendNotification);
    bassLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    bassLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(bassLabel);
    
    otherLabel.setText("Other", juce::dontSendNotification);
    otherLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    otherLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(otherLabel);
    
    vocalLabel.setText("Vocals", juce::dontSendNotification);
    vocalLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    vocalLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(vocalLabel);
    
    qualityLabel.setText("Quality", juce::dontSendNotification);
    qualityLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(qualityLabel);
    
    outputModeLabel.setText("Output Mode", juce::dontSendNotification);
    outputModeLabel.setFont(juce::Font(12.0f, juce::Font::bold));
    addAndMakeVisible(outputModeLabel);
    
    // Connect parameters to GUI
    drumAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "drumLevel", drumSlider);
    bassAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "bassLevel", bassSlider);
    otherAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "otherLevel", otherSlider);
    vocalAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "vocalLevel", vocalSlider);
    qualityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        audioProcessor.parameters, "quality", qualitySlider);
    outputModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        audioProcessor.parameters, "outputMode", outputModeCombo);
    
    setSize (400, 300);
}

StemSplitterSamplerAudioProcessorEditor::~StemSplitterSamplerAudioProcessorEditor()
{
}

//==============================================================================
void StemSplitterSamplerAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Stem Splitter Sampler", getLocalBounds(), juce::Justification::centredTop, 1);
}

void StemSplitterSamplerAudioProcessorEditor::resized()
{
    auto area = getLocalBounds();
    auto titleArea = area.removeFromTop(30);
    
    // Arrange sliders horizontally
    auto sliderArea = area.removeFromTop(180);
    int sliderWidth = sliderArea.getWidth() / 5;
    
    drumSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    bassSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    otherSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    vocalSlider.setBounds(sliderArea.removeFromLeft(sliderWidth));
    
    // Labels under sliders
    auto labelArea = area.removeFromTop(30);
    int labelWidth = labelArea.getWidth() / 5;
    drumLabel.setBounds(labelArea.removeFromLeft(labelWidth));
    bassLabel.setBounds(labelArea.removeFromLeft(labelWidth));
    otherLabel.setBounds(labelArea.removeFromLeft(labelWidth));
    vocalLabel.setBounds(labelArea.removeFromLeft(labelWidth));
    
    // Quality slider and output mode at bottom
    auto controlArea = area;
    auto qualityArea = controlArea.removeFromLeft(200);
    qualitySlider.setBounds(qualityArea.removeFromTop(30));
    qualityLabel.setBounds(qualityArea);
    
    outputModeLabel.setBounds(controlArea.removeFromLeft(100));
    outputModeCombo.setBounds(controlArea.removeFromLeft(100));
}
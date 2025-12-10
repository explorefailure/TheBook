#pragma once
#include "PluginProcessor.h"

class BitcrusherAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit BitcrusherAudioProcessorEditor(BitcrusherAudioProcessor&);
    ~BitcrusherAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    BitcrusherAudioProcessor& processorRef;

    // Controls
    juce::Slider bitsSlider, rateSlider, mixSlider;
    juce::Label bitsLabel, rateLabel, mixLabel, titleLabel;

    // Attachments (declared AFTER sliders)
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> bitsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BitcrusherAudioProcessorEditor)
};

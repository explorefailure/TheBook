#pragma once
#include "PluginProcessor.h"

class ChopShopAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit ChopShopAudioProcessorEditor(ChopShopAudioProcessor&);
    ~ChopShopAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    ChopShopAudioProcessor& processorRef;

    // Controls
    juce::ComboBox stutterRateBox;
    juce::Slider pitchSlider, speedSlider, thresholdSlider, mixSlider;
    juce::ToggleButton linkButton, reverseButton;
    juce::ComboBox triggerModeBox;

    // Labels
    juce::Label titleLabel;
    juce::Label stutterRateLabel, pitchLabel, speedLabel, linkLabel;
    juce::Label reverseLabel, triggerModeLabel, thresholdLabel, mixLabel;

    // Attachments (order matters: declare AFTER controls)
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> stutterRateAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pitchAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> speedAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> linkAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> reverseAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> triggerModeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChopShopAudioProcessorEditor)
};

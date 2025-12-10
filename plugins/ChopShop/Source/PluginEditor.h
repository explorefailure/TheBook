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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChopShopAudioProcessorEditor)
};

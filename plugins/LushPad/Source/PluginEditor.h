#pragma once
#include "PluginProcessor.h"

class LushPadAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit LushPadAudioProcessorEditor(LushPadAudioProcessor&);
    ~LushPadAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    LushPadAudioProcessor& processorRef;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LushPadAudioProcessorEditor)
};

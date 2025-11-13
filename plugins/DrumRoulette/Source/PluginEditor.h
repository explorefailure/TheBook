#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

class DrumRouletteAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit DrumRouletteAudioProcessorEditor(DrumRouletteAudioProcessor&);
    ~DrumRouletteAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    DrumRouletteAudioProcessor& processorRef;

    // WebView member (use std::unique_ptr - Pattern #11)
    std::unique_ptr<juce::WebBrowserComponent> webView;

    // Helper for resource serving (Pattern #8 - explicit URL mapping)
    std::optional<juce::WebBrowserComponent::Resource> getResource(const juce::String& url);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DrumRouletteAudioProcessorEditor)
};

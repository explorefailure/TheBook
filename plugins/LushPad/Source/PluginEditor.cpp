#include "PluginEditor.h"

LushPadAudioProcessorEditor::LushPadAudioProcessorEditor(LushPadAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 400);
}

LushPadAudioProcessorEditor::~LushPadAudioProcessorEditor()
{
}

void LushPadAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("LushPad - Stage 2", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("3 parameters implemented",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);
}

void LushPadAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 4 (GUI)
}

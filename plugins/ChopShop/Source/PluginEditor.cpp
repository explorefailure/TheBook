#include "PluginEditor.h"

//==============================================================================
ChopShopAudioProcessorEditor::ChopShopAudioProcessorEditor(ChopShopAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(600, 400);
}

ChopShopAudioProcessorEditor::~ChopShopAudioProcessorEditor()
{
}

//==============================================================================
void ChopShopAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));

    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawFittedText("ChopShop - Stage 1", getLocalBounds(), juce::Justification::centred, 1);

    g.setFont(14.0f);
    g.drawFittedText("8 parameters + chain order implemented",
                     getLocalBounds().reduced(20).removeFromBottom(30),
                     juce::Justification::centred, 1);
}

void ChopShopAudioProcessorEditor::resized()
{
    // Layout will be added in Stage 2 (GUI)
}

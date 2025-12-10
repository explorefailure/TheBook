#include "PluginEditor.h"
#include "PluginIDs.h"

//==============================================================================
ChopShopAudioProcessorEditor::ChopShopAudioProcessorEditor(ChopShopAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Title label
    titleLabel.setText("ChopShop - Stutter/Glitch Effect", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(20.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // === Stutter Rate (ComboBox) ===
    stutterRateBox.addItem("1/1", 1);
    stutterRateBox.addItem("1/2", 2);
    stutterRateBox.addItem("1/4", 3);
    stutterRateBox.addItem("1/8", 4);
    stutterRateBox.addItem("1/16", 5);
    stutterRateBox.addItem("1/32", 6);
    stutterRateBox.addItem("1/64", 7);
    addAndMakeVisible(stutterRateBox);

    stutterRateLabel.setText("Stutter Rate", juce::dontSendNotification);
    stutterRateLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(stutterRateLabel);

    stutterRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.parameters, PluginIDs::Params::STUTTER_RATE, stutterRateBox);

    // === Pitch Shift (Slider) ===
    pitchSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    pitchSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    pitchSlider.setTextValueSuffix(" st");
    addAndMakeVisible(pitchSlider);

    pitchLabel.setText("Pitch Shift", juce::dontSendNotification);
    pitchLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(pitchLabel);

    pitchAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, PluginIDs::Params::PITCH_SHIFT, pitchSlider);

    // === Speed (Slider) ===
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.setTextValueSuffix(" %");
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(speedLabel);

    speedAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, PluginIDs::Params::SPEED, speedSlider);

    // === Link Pitch/Speed (ToggleButton) ===
    linkButton.setButtonText("Link Pitch/Speed");
    addAndMakeVisible(linkButton);

    linkAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.parameters, PluginIDs::Params::LINK_PITCH_SPEED, linkButton);

    // === Reverse (ToggleButton) ===
    reverseButton.setButtonText("Reverse");
    addAndMakeVisible(reverseButton);

    reverseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processorRef.parameters, PluginIDs::Params::REVERSE, reverseButton);

    // === Trigger Mode (ComboBox) ===
    triggerModeBox.addItem("Always", 1);
    triggerModeBox.addItem("MIDI", 2);
    triggerModeBox.addItem("Threshold", 3);
    addAndMakeVisible(triggerModeBox);

    triggerModeLabel.setText("Trigger Mode", juce::dontSendNotification);
    triggerModeLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(triggerModeLabel);

    triggerModeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processorRef.parameters, PluginIDs::Params::TRIGGER_MODE, triggerModeBox);

    // === Threshold (Slider) ===
    thresholdSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    thresholdSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    thresholdSlider.setTextValueSuffix(" dB");
    addAndMakeVisible(thresholdSlider);

    thresholdLabel.setText("Threshold", juce::dontSendNotification);
    thresholdLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(thresholdLabel);

    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, PluginIDs::Params::THRESHOLD, thresholdSlider);

    // === Mix (Slider) ===
    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    mixSlider.setTextValueSuffix(" %");
    addAndMakeVisible(mixSlider);

    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.setJustificationType(juce::Justification::centredLeft);
    addAndMakeVisible(mixLabel);

    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, PluginIDs::Params::MIX, mixSlider);

    setSize(600, 400);
}

ChopShopAudioProcessorEditor::~ChopShopAudioProcessorEditor()
{
}

//==============================================================================
void ChopShopAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void ChopShopAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);
    auto titleHeight = 40;
    auto rowHeight = 35;
    auto labelWidth = 120;

    // Title at top
    titleLabel.setBounds(bounds.removeFromTop(titleHeight));
    bounds.removeFromTop(10); // Spacing

    // Helper lambda for laying out rows
    auto layoutRow = [&](juce::Label& label, juce::Component& control) {
        auto row = bounds.removeFromTop(rowHeight);
        label.setBounds(row.removeFromLeft(labelWidth));
        control.setBounds(row.reduced(5, 5));
    };

    // Layout controls in order
    layoutRow(stutterRateLabel, stutterRateBox);
    layoutRow(pitchLabel, pitchSlider);
    layoutRow(speedLabel, speedSlider);

    // Toggle buttons (no separate label - button has text)
    auto toggleRow = bounds.removeFromTop(rowHeight);
    linkButton.setBounds(toggleRow.removeFromLeft(200).reduced(5, 5));
    reverseButton.setBounds(toggleRow.reduced(5, 5));

    layoutRow(triggerModeLabel, triggerModeBox);
    layoutRow(thresholdLabel, thresholdSlider);
    layoutRow(mixLabel, mixSlider);
}

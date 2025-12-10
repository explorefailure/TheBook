#include "PluginEditor.h"

BitcrusherAudioProcessorEditor::BitcrusherAudioProcessorEditor(BitcrusherAudioProcessor& p)
    : juce::AudioProcessorEditor(p), processorRef(p)
{
    // Title
    titleLabel.setText("Bitcrusher", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Bits slider
    bitsSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bitsSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    addAndMakeVisible(bitsSlider);

    bitsLabel.setText("Bits", juce::dontSendNotification);
    bitsLabel.attachToComponent(&bitsSlider, true);
    addAndMakeVisible(bitsLabel);

    // Rate slider
    rateSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    rateSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    rateSlider.setTextValueSuffix(" Hz");
    addAndMakeVisible(rateSlider);

    rateLabel.setText("Rate", juce::dontSendNotification);
    rateLabel.attachToComponent(&rateSlider, true);
    addAndMakeVisible(rateLabel);

    // Mix slider
    mixSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    mixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    mixSlider.setTextValueSuffix(" %");
    addAndMakeVisible(mixSlider);

    mixLabel.setText("Mix", juce::dontSendNotification);
    mixLabel.attachToComponent(&mixSlider, true);
    addAndMakeVisible(mixLabel);

    // Create attachments (AFTER sliders are configured)
    bitsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "bits", bitsSlider);
    rateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "rate", rateSlider);
    mixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processorRef.parameters, "mix", mixSlider);

    setSize(400, 200);
}

BitcrusherAudioProcessorEditor::~BitcrusherAudioProcessorEditor()
{
}

void BitcrusherAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff2d2d2d));
}

void BitcrusherAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds().reduced(20);

    // Title at top
    titleLabel.setBounds(bounds.removeFromTop(40));

    bounds.removeFromTop(10); // Spacing

    auto labelWidth = 60;
    auto rowHeight = 35;

    // Bits row
    auto row = bounds.removeFromTop(rowHeight);
    bitsSlider.setBounds(row.withTrimmedLeft(labelWidth));

    bounds.removeFromTop(5);

    // Rate row
    row = bounds.removeFromTop(rowHeight);
    rateSlider.setBounds(row.withTrimmedLeft(labelWidth));

    bounds.removeFromTop(5);

    // Mix row
    row = bounds.removeFromTop(rowHeight);
    mixSlider.setBounds(row.withTrimmedLeft(labelWidth));
}

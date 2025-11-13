#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout LushPadAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // timbre - Float (0.0 to 1.0, default: 0.35, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "timbre", 1 },
        "Timbre",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
        0.35f
    ));

    // filter_cutoff - Float (20.0 to 20000.0 Hz, default: 2000.0, skew: 0.3 logarithmic)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "filter_cutoff", 1 },
        "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 0.1f, 0.3f),
        2000.0f,
        "Hz"
    ));

    // reverb_amount - Float (0.0 to 1.0, default: 0.4, linear)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "reverb_amount", 1 },
        "Reverb Amount",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f, 1.0f),
        0.4f
    ));

    return layout;
}

LushPadAudioProcessor::LushPadAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

LushPadAudioProcessor::~LushPadAudioProcessor()
{
}

void LushPadAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 3 (DSP)
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void LushPadAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 3 (DSP)
}

void LushPadAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Parameter access example (for Stage 3 DSP implementation):
    // auto* timbreParam = parameters.getRawParameterValue("timbre");
    // float timbreValue = timbreParam->load();  // Atomic read (real-time safe)

    // auto* filterCutoffParam = parameters.getRawParameterValue("filter_cutoff");
    // float filterCutoffValue = filterCutoffParam->load();

    // auto* reverbAmountParam = parameters.getRawParameterValue("reverb_amount");
    // float reverbAmountValue = reverbAmountParam->load();

    // Pass-through for Stage 2 (DSP implementation happens in Stage 3)
    // Audio routing is already handled by JUCE
}

juce::AudioProcessorEditor* LushPadAudioProcessor::createEditor()
{
    return new LushPadAudioProcessorEditor(*this);
}

void LushPadAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LushPadAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LushPadAudioProcessor();
}

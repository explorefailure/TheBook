#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "PluginIDs.h"

//==============================================================================
// Parameter Layout Creation
//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout ChopShopAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // Stutter Rate (Choice: 1/1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { PluginIDs::Params::STUTTER_RATE, 1 },
        "Stutter Rate",
        juce::StringArray { "1/1", "1/2", "1/4", "1/8", "1/16", "1/32", "1/64" },
        3  // Default: 1/8 (index 3)
    ));

    // Pitch Shift (Float: -24.0 to +24.0 semitones)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { PluginIDs::Params::PITCH_SHIFT, 1 },
        "Pitch Shift",
        juce::NormalisableRange<float>(-24.0f, 24.0f, 0.1f),
        0.0f,
        "st"
    ));

    // Speed (Float: 50.0 to 200.0 percent)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { PluginIDs::Params::SPEED, 1 },
        "Speed",
        juce::NormalisableRange<float>(50.0f, 200.0f, 0.1f),
        100.0f,
        "%"
    ));

    // Link Pitch/Speed (Bool: On/Off)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { PluginIDs::Params::LINK_PITCH_SPEED, 1 },
        "Link Pitch/Speed",
        false  // Default: Off (independent mode)
    ));

    // Reverse (Bool: On/Off)
    layout.add(std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { PluginIDs::Params::REVERSE, 1 },
        "Reverse",
        false  // Default: Off
    ));

    // Trigger Mode (Choice: Always, MIDI, Threshold)
    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { PluginIDs::Params::TRIGGER_MODE, 1 },
        "Trigger Mode",
        juce::StringArray { "Always", "MIDI", "Threshold" },
        0  // Default: Always (index 0)
    ));

    // Threshold (Float: -60.0 to 0.0 dB)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { PluginIDs::Params::THRESHOLD, 1 },
        "Threshold",
        juce::NormalisableRange<float>(-60.0f, 0.0f, 0.1f),
        -20.0f,
        "dB"
    ));

    // Mix (Float: 0.0 to 100.0 percent)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { PluginIDs::Params::MIX, 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"
    ));

    return layout;
}

//==============================================================================
// Constructor / Destructor
//==============================================================================
ChopShopAudioProcessor::ChopShopAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
    , customState("PluginState")
{
    // Initialize custom state (chain order)
    customState.setProperty(PluginIDs::Params::CHAIN_ORDER, "0,1,2,3", nullptr);
}

ChopShopAudioProcessor::~ChopShopAudioProcessor()
{
}

//==============================================================================
// Audio Processing
//==============================================================================
void ChopShopAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Initialization will be added in Stage 2 (DSP)
    juce::ignoreUnused(sampleRate, samplesPerBlock);
}

void ChopShopAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 2 (DSP)
}

void ChopShopAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Parameter access example (for Stage 2 DSP implementation):
    // auto* stutterRateParam = parameters.getRawParameterValue(PluginIDs::Params::STUTTER_RATE);
    // int stutterRateIndex = static_cast<int>(stutterRateParam->load());  // Atomic read

    // Pass-through for Stage 1 (DSP implementation happens in Stage 2)
    // Audio routing is already handled by JUCE
}

//==============================================================================
// Editor
//==============================================================================
juce::AudioProcessorEditor* ChopShopAudioProcessor::createEditor()
{
    return new ChopShopAudioProcessorEditor(*this);
}

//==============================================================================
// State Management
//==============================================================================
void ChopShopAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // Save APVTS parameters (automatic)
    auto state = parameters.copyState();

    // Add custom state (chain order)
    state.appendChild(customState.createCopy(), nullptr);

    // Serialize to XML
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void ChopShopAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
    {
        auto restoredState = juce::ValueTree::fromXml(*xmlState);
        parameters.replaceState(restoredState);

        // Restore custom state (chain order)
        auto customChild = restoredState.getChildWithName("PluginState");
        if (customChild.isValid())
        {
            customState = customChild.createCopy();
        }
        else
        {
            // Fallback to default chain order
            customState.setProperty(PluginIDs::Params::CHAIN_ORDER, "0,1,2,3", nullptr);
        }
    }
}

//==============================================================================
// Plugin Factory
//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ChopShopAudioProcessor();
}

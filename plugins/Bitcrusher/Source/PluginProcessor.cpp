#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <juce_dsp/juce_dsp.h>

juce::AudioProcessorValueTreeState::ParameterLayout BitcrusherAudioProcessor::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    // bits - Bit depth reduction (1-24 bits)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "bits", 1 },
        "Bits",
        juce::NormalisableRange<float>(1.0f, 24.0f, 0.1f),
        8.0f
    ));

    // rate - Sample rate reduction (100-44100 Hz, logarithmic)
    juce::NormalisableRange<float> rateRange(100.0f, 44100.0f);
    rateRange.setSkewForCentre(2000.0f);  // Centre at 2000 Hz for natural feel
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "rate", 1 },
        "Rate",
        rateRange,
        4000.0f,
        "Hz"
    ));

    // mix - Dry/wet blend (0-100%)
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "mix", 1 },
        "Mix",
        juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f),
        100.0f,
        "%"
    ));

    return layout;
}

BitcrusherAudioProcessor::BitcrusherAudioProcessor()
    : AudioProcessor(BusesProperties()
                        .withInput("Input", juce::AudioChannelSet::stereo(), true)
                        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
    , parameters(*this, nullptr, "Parameters", createParameterLayout())
{
}

BitcrusherAudioProcessor::~BitcrusherAudioProcessor()
{
}

void BitcrusherAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Cache sample rate
    currentSampleRate = sampleRate;

    // Prepare dry/wet mixer
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    dryWetMixer.prepare(spec);
    dryWetMixer.reset();

    // Reset sample-and-hold state
    heldSampleL = 0.0f;
    heldSampleR = 0.0f;
    sampleCounter = 0.0f;
}

void BitcrusherAudioProcessor::releaseResources()
{
    // Cleanup will be added in Stage 2 (DSP)
}

void BitcrusherAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused(midiMessages);

    // Clear unused channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Read parameters (atomic, real-time safe)
    auto* bitsParam = parameters.getRawParameterValue("bits");
    float bitsValue = bitsParam->load();

    auto* rateParam = parameters.getRawParameterValue("rate");
    float rateValue = rateParam->load();

    auto* mixParam = parameters.getRawParameterValue("mix");
    float mixValue = mixParam->load() / 100.0f;  // Convert percentage to 0-1

    // Push dry signal to mixer
    juce::dsp::AudioBlock<float> block(buffer);
    dryWetMixer.pushDrySamples(block);

    // Process audio - bit depth and sample rate reduction
    const int numChannels = buffer.getNumChannels();
    const int numSamples = buffer.getNumSamples();

    // Calculate bit reduction scale
    const float scale = std::pow(2.0f, bitsValue - 1.0f);
    const float invScale = 1.0f / scale;

    // Calculate sample-and-hold interval
    const float holdLength = static_cast<float>(currentSampleRate) / rateValue;

    // Process each sample
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Sample-and-hold: Hold samples to simulate lower sample rate
        sampleCounter += 1.0f;
        if (sampleCounter >= holdLength)
        {
            // Time to grab new samples
            if (numChannels > 0)
                heldSampleL = *buffer.getReadPointer(0, sample);
            if (numChannels > 1)
                heldSampleR = *buffer.getReadPointer(1, sample);

            sampleCounter = 0.0f;
        }

        // Apply held samples
        if (numChannels > 0)
        {
            // Bit reduction: Quantize to fewer bits
            float reduced = std::round(heldSampleL * scale) * invScale;
            *buffer.getWritePointer(0, sample) = reduced;
        }

        if (numChannels > 1)
        {
            // Bit reduction: Quantize to fewer bits
            float reduced = std::round(heldSampleR * scale) * invScale;
            *buffer.getWritePointer(1, sample) = reduced;
        }
    }

    // Mix dry/wet
    dryWetMixer.setWetMixProportion(mixValue);
    dryWetMixer.mixWetSamples(block);
}

juce::AudioProcessorEditor* BitcrusherAudioProcessor::createEditor()
{
    return new BitcrusherAudioProcessorEditor(*this);
}

void BitcrusherAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void BitcrusherAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr && xmlState->hasTagName(parameters.state.getType()))
        parameters.replaceState(juce::ValueTree::fromXml(*xmlState));
}

// Factory function
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BitcrusherAudioProcessor();
}

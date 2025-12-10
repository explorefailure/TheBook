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
// Helper Methods
//==============================================================================
float ChopShopAudioProcessor::getDivisionValue(int stutterRateIndex) const
{
    // 0=1/1, 1=1/2, 2=1/4, 3=1/8, 4=1/16, 5=1/32, 6=1/64
    const float divisions[] = { 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f, 64.0f };
    return divisions[juce::jlimit(0, 6, stutterRateIndex)];
}

void ChopShopAudioProcessor::captureAudio(juce::AudioBuffer<float>& buffer)
{
    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin(buffer.getNumChannels(), stutterBuffer.getNumChannels());

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* sourceData = buffer.getReadPointer(channel);
        auto* destData = stutterBuffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            destData[capturePosition] = sourceData[sample];
            capturePosition = (capturePosition + 1) % stutterBufferSize;
        }
    }
}

void ChopShopAudioProcessor::playbackStutter(juce::AudioBuffer<float>& buffer, bool reverseEnabled)
{
    if (segmentLength <= 0)
        return;

    const int numSamples = buffer.getNumSamples();
    const int numChannels = juce::jmin(buffer.getNumChannels(), stutterBuffer.getNumChannels());
    const int crossfadeSamples = static_cast<int>(currentSampleRate * 0.005); // 5ms

    for (int channel = 0; channel < numChannels; ++channel)
    {
        auto* outputData = buffer.getWritePointer(channel);
        auto* stutterData = stutterBuffer.getReadPointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Phase 2.2: Read forward or backward based on reverse parameter
            int readPos;
            if (reverseEnabled)
            {
                // Reverse playback: read from end to beginning
                readPos = (segmentLength - 1) - (playbackPosition % segmentLength);
            }
            else
            {
                // Normal playback: read from beginning to end
                readPos = playbackPosition % segmentLength;
            }

            float outputSample = stutterData[readPos];

            // Apply crossfade at loop boundaries
            int positionInSegment = playbackPosition % segmentLength;

            // Fade out last 5ms of segment
            if (positionInSegment >= (segmentLength - crossfadeSamples))
            {
                int fadeOutPos = positionInSegment - (segmentLength - crossfadeSamples);
                float fadeOutGain = 1.0f - (static_cast<float>(fadeOutPos) / crossfadeSamples);

                // Also get fade-in sample from start of segment
                int fadeInReadPos;
                if (reverseEnabled)
                {
                    fadeInReadPos = (segmentLength - 1) - fadeOutPos;
                }
                else
                {
                    fadeInReadPos = fadeOutPos;
                }

                float fadeInSample = stutterData[fadeInReadPos];
                float fadeInGain = static_cast<float>(fadeOutPos) / crossfadeSamples;

                // Crossfade
                outputSample = outputSample * fadeOutGain + fadeInSample * fadeInGain;
            }

            outputData[sample] = outputSample;
            playbackPosition = (playbackPosition + 1) % segmentLength;
        }
    }
}

void ChopShopAudioProcessor::triggerCapture()
{
    // Reset playback position to restart stutter loop from beginning
    playbackPosition = 0;
}

//==============================================================================
// Phase 2.3: Effect Chain Methods
//==============================================================================

void ChopShopAudioProcessor::parseChainOrder()
{
    // Parse chainOrder string from ValueTree
    juce::String orderStr = customState.getProperty(PluginIDs::Params::CHAIN_ORDER, "0,1,2,3").toString();
    juce::StringArray tokens;
    tokens.addTokens(orderStr, ",", "");

    // Fill effectOrder array (default to 0,1,2,3 if parsing fails)
    for (int i = 0; i < 4; ++i)
    {
        if (i < tokens.size())
        {
            effectOrder[i] = tokens[i].getIntValue();
        }
        else
        {
            effectOrder[i] = i;  // Fallback to default order
        }
    }
}

void ChopShopAudioProcessor::applyStutter(juce::AudioBuffer<float>& buffer)
{
    // Stutter is already applied via playbackStutter() in main processBlock
    // This method is a placeholder for the effect chain architecture
    // Actual stutter processing happens earlier in the chain
}

void ChopShopAudioProcessor::applyPitchShift(
    juce::AudioBuffer<float>& buffer,
    float pitchSemitones,
    float speedPercent,
    bool linked)
{
    // Early exit if no processing needed
    if (std::abs(pitchSemitones) < 0.01f && std::abs(speedPercent - 100.0f) < 0.1f)
        return;

    // Calculate combined ratio
    float pitchRatio = std::pow(2.0f, pitchSemitones / 12.0f);
    float speedRatio = speedPercent / 100.0f;

    float combinedRatio;
    if (linked)
    {
        // Linked mode: speed affects pitch (varispeed/tape-style)
        combinedRatio = pitchRatio * speedRatio;
    }
    else
    {
        // Independent mode: for now, just use pitch ratio
        // Full time-stretching would go here in future enhancement
        combinedRatio = pitchRatio;
    }

    // Simple resampling (linear interpolation)
    processBuffer.clear();
    int numSamples = buffer.getNumSamples();
    int numChannels = juce::jmin(buffer.getNumChannels(), processBuffer.getNumChannels());

    for (int channel = 0; channel < numChannels; ++channel)
    {
        const float* input = buffer.getReadPointer(channel);
        float* output = processBuffer.getWritePointer(channel);

        float readPos = 0.0f;
        for (int i = 0; i < numSamples; ++i)
        {
            int idx0 = static_cast<int>(readPos);
            int idx1 = idx0 + 1;
            float frac = readPos - idx0;

            // Clamp indices to valid range
            idx0 = juce::jlimit(0, numSamples - 1, idx0);
            idx1 = juce::jlimit(0, numSamples - 1, idx1);

            // Linear interpolation
            output[i] = input[idx0] * (1.0f - frac) + input[idx1] * frac;

            readPos += combinedRatio;
            if (readPos >= numSamples - 1)
                readPos = numSamples - 1.0f;
        }
    }

    // Copy processed audio back to buffer
    for (int ch = 0; ch < numChannels; ++ch)
    {
        buffer.copyFrom(ch, 0, processBuffer, ch, 0, numSamples);
    }
}

void ChopShopAudioProcessor::applyReverse(juce::AudioBuffer<float>& buffer)
{
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        float* data = buffer.getWritePointer(channel);
        std::reverse(data, data + numSamples);
    }
}

//==============================================================================
// Audio Processing
//==============================================================================
void ChopShopAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    // Prepare DSP spec
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    // Prepare dry/wet mixer
    dryWetMixer.prepare(spec);
    dryWetMixer.reset();

    // Allocate stutter buffer (4 bars at minimum 40 BPM @ max sample rate)
    // Calculation: (sampleRate * 60.0 / minBPM) * 4.0 * 4.0
    const double minBPM = 40.0;
    stutterBufferSize = static_cast<int>((sampleRate * 60.0 / minBPM) * 4.0 * 4.0);
    stutterBuffer.setSize(getTotalNumOutputChannels(), stutterBufferSize);
    stutterBuffer.clear();

    // Initialize positions
    capturePosition = 0;
    playbackPosition = 0;

    // Calculate initial segment length (default: 1/8 note at 120 BPM)
    const float defaultDivision = 8.0f; // 1/8 note
    segmentLength = static_cast<int>((sampleRate * 60.0 / currentBPM) * (4.0 / defaultDivision));

    // Phase 2.2: Calculate envelope follower coefficients
    // Fast envelope: 1ms attack, 10ms release
    fastAttackCoeff = std::exp(-1.0f / static_cast<float>(sampleRate * 0.001));
    fastReleaseCoeff = std::exp(-1.0f / static_cast<float>(sampleRate * 0.010));

    // Slow envelope: 50ms attack, 200ms release
    slowAttackCoeff = std::exp(-1.0f / static_cast<float>(sampleRate * 0.050));
    slowReleaseCoeff = std::exp(-1.0f / static_cast<float>(sampleRate * 0.200));

    // Reset envelope states
    fastEnvelope[0] = fastEnvelope[1] = 0.0f;
    slowEnvelope[0] = slowEnvelope[1] = 0.0f;
    triggered = false;

    // Phase 2.3: Allocate process buffer
    processBuffer.setSize(getTotalNumOutputChannels(), samplesPerBlock);

    // Initialize effect order
    parseChainOrder();
}

void ChopShopAudioProcessor::releaseResources()
{
    // Release stutter buffer to save memory
    stutterBuffer.setSize(0, 0);
}

void ChopShopAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    // Clear unused channels
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Get BPM from host (with fallback to 120 BPM)
    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            currentBPM = position->getBpm().orFallback(120.0);
        }
    }

    // Read parameters (atomic)
    auto* stutterRateParam = parameters.getRawParameterValue(PluginIDs::Params::STUTTER_RATE);
    int stutterRateIndex = static_cast<int>(stutterRateParam->load());
    float divisionValue = getDivisionValue(stutterRateIndex);

    auto* mixParam = parameters.getRawParameterValue(PluginIDs::Params::MIX);
    float mixValue = mixParam->load() / 100.0f; // Convert 0-100 to 0-1

    auto* triggerModeParam = parameters.getRawParameterValue(PluginIDs::Params::TRIGGER_MODE);
    int triggerMode = static_cast<int>(triggerModeParam->load()); // 0=Always, 1=MIDI, 2=Threshold

    auto* thresholdParam = parameters.getRawParameterValue(PluginIDs::Params::THRESHOLD);
    float thresholdDB = thresholdParam->load(); // -60dB to 0dB

    auto* reverseParam = parameters.getRawParameterValue(PluginIDs::Params::REVERSE);
    bool reverseEnabled = reverseParam->load() > 0.5f;

    // Calculate segment length based on tempo and division
    segmentLength = static_cast<int>((currentSampleRate * 60.0 / currentBPM) * (4.0 / divisionValue));
    segmentLength = juce::jlimit(1, stutterBufferSize, segmentLength);

    // Push dry signal to mixer
    dryWetMixer.pushDrySamples(buffer);

    // Phase 2.2: Trigger mode logic
    bool shouldTrigger = false;

    // MIDI Trigger Mode
    if (triggerMode == 1)
    {
        for (const auto metadata : midiMessages)
        {
            auto msg = metadata.getMessage();
            if (msg.isNoteOn())
            {
                shouldTrigger = true;
                break;
            }
        }
    }
    // Threshold Trigger Mode
    else if (triggerMode == 2)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            for (int channel = 0; channel < juce::jmin(numChannels, 2); ++channel)
            {
                auto* channelData = buffer.getReadPointer(channel);
                float inputLevel = std::abs(channelData[sample]);

                // Update fast envelope
                if (inputLevel > fastEnvelope[channel])
                {
                    fastEnvelope[channel] = fastAttackCoeff * fastEnvelope[channel] + (1.0f - fastAttackCoeff) * inputLevel;
                }
                else
                {
                    fastEnvelope[channel] = fastReleaseCoeff * fastEnvelope[channel] + (1.0f - fastReleaseCoeff) * inputLevel;
                }

                // Update slow envelope
                if (inputLevel > slowEnvelope[channel])
                {
                    slowEnvelope[channel] = slowAttackCoeff * slowEnvelope[channel] + (1.0f - slowAttackCoeff) * inputLevel;
                }
                else
                {
                    slowEnvelope[channel] = slowReleaseCoeff * slowEnvelope[channel] + (1.0f - slowReleaseCoeff) * inputLevel;
                }

                // Convert to dB and detect transient
                float fastDB = 20.0f * std::log10(std::max(fastEnvelope[channel], 1e-6f));
                float slowDB = 20.0f * std::log10(std::max(slowEnvelope[channel], 1e-6f));
                float delta = fastDB - slowDB;

                // Trigger on rising transient (with hysteresis)
                if (delta > thresholdDB && !triggered)
                {
                    shouldTrigger = true;
                    triggered = true;
                }

                // Reset trigger state when delta drops below threshold (with 3dB hysteresis)
                if (delta < (thresholdDB - 3.0f))
                {
                    triggered = false;
                }
            }
        }
    }

    // Capture audio (continuous for all modes)
    captureAudio(buffer);

    // Trigger capture on MIDI or threshold detection
    if (shouldTrigger)
    {
        triggerCapture();
    }

    // Playback stuttered audio (base processing before effect chain)
    playbackStutter(buffer, false);  // Reverse handled in effect chain

    // Phase 2.3: Apply effect chain in configured order
    auto* pitchShiftParam = parameters.getRawParameterValue(PluginIDs::Params::PITCH_SHIFT);
    float pitchSemitones = pitchShiftParam->load();

    auto* speedParam = parameters.getRawParameterValue(PluginIDs::Params::SPEED);
    float speedPercent = speedParam->load();

    auto* linkPitchSpeedParam = parameters.getRawParameterValue(PluginIDs::Params::LINK_PITCH_SPEED);
    bool linked = linkPitchSpeedParam->load() > 0.5f;

    // Apply effects in chain order
    for (int i = 0; i < 4; ++i)
    {
        int effectIndex = effectOrder[i];
        switch (effectIndex)
        {
            case 0: // Stutter - already applied via playbackStutter()
                applyStutter(buffer);
                break;

            case 1: // Pitch Shift
                applyPitchShift(buffer, pitchSemitones, speedPercent, linked);
                break;

            case 2: // Speed - handled together with pitch in linked mode
                // In linked mode, speed is applied with pitch
                // In independent mode, would need separate time-stretch (future enhancement)
                break;

            case 3: // Reverse
                if (reverseEnabled)
                    applyReverse(buffer);
                break;

            default:
                break;
        }
    }

    // Mix dry/wet
    dryWetMixer.setWetMixProportion(mixValue);
    dryWetMixer.mixWetSamples(buffer);
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

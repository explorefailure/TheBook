# Dry/Wet Mixing

## Overview

Dry/wet mixing blends the original (dry) signal with the processed (wet) signal. Essential for parallel processing effects where you want to preserve some of the original character while adding the effect.

## Use Cases

- Distortion and saturation (parallel distortion)
- Compression (parallel/New York compression)
- Reverb and delay
- Any effect where 100% wet may be too extreme

## JUCE Implementation

### Required Modules

- `juce::dsp::DryWetMixer`

### Code Pattern

```cpp
// Header
juce::dsp::DryWetMixer<float> dryWetMixer { 512 };  // Max samples per block

// prepareToPlay
void prepareToPlay(double sampleRate, int samplesPerBlock) override
{
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = static_cast<juce::uint32>(getTotalNumOutputChannels());

    dryWetMixer.prepare(spec);
    dryWetMixer.reset();
}

// processBlock
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
{
    // Step 1: Capture dry signal BEFORE any processing
    dryWetMixer.pushDrySamples(buffer);

    // Step 2: Process the buffer (distortion, etc.)
    // ... your DSP here ...

    // Step 3: Set mix amount and blend
    dryWetMixer.setWetMixProportion(mixValue);  // 0.0 = dry, 1.0 = wet
    dryWetMixer.mixWetSamples(buffer);
}
```

### With Oversampling

When using oversampling, capture dry signal BEFORE upsampling and mix AFTER downsampling:

```cpp
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
{
    // 1. Capture dry (original sample rate)
    dryWetMixer.pushDrySamples(buffer);

    // 2. Upsample
    juce::dsp::AudioBlock<float> block(buffer);
    auto oversampledBlock = oversampling.processSamplesUp(block);

    // 3. Process at oversampled rate
    // ... DSP ...

    // 4. Downsample
    oversampling.processSamplesDown(block);

    // 5. Mix (original sample rate)
    dryWetMixer.setWetMixProportion(mixValue);
    dryWetMixer.mixWetSamples(buffer);
}
```

### Gotchas

1. **Latency compensation is automatic**: `DryWetMixer` handles latency internally when you use `setLatencySamples()` on the processor.

2. **Call pushDrySamples FIRST**: Must capture before any modification to the buffer.

3. **Don't manually compensate latency**: The mixer does this for you based on reported latency.

4. **Prepare with correct spec**: Must match your actual channel count and block size.

5. **Mix parameter is 0-1**: Not percentage. 0.5 = 50% mix.

## Parameters

| Value | Result |
|-------|--------|
| 0.0 | 100% dry (bypass) |
| 0.5 | 50/50 blend |
| 1.0 | 100% wet |

Typical default: 0.5 for parallel effects, 1.0 for serial effects (reverb, delay)

## Manual Implementation

If you need more control (e.g., different curves), manual mixing:

```cpp
// Simple linear mix
for (int ch = 0; ch < numChannels; ++ch)
{
    auto* dry = dryBuffer.getReadPointer(ch);
    auto* wet = buffer.getWritePointer(ch);

    for (int i = 0; i < numSamples; ++i)
    {
        wet[i] = dry[i] * (1.0f - mixValue) + wet[i] * mixValue;
    }
}
```

But this doesn't handle latency - use `DryWetMixer` unless you have a specific reason not to.

## CPU Considerations

- Very cheap - just multiplication and addition
- Memory for dry buffer (one block's worth)
- Negligible compared to actual DSP processing

## References

- JUCE DryWetMixer documentation
- Parallel compression techniques
- Mix knob on every guitar pedal ever made

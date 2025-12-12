# Oversampling

## Overview

Oversampling reduces aliasing artifacts from nonlinear processing (distortion, saturation, waveshaping) by processing at a higher sample rate then filtering and downsampling back to the original rate.

## Use Cases

- Waveshapers and wave folders
- Distortion and saturation effects
- Clippers and limiters
- Any nonlinear transfer function

## JUCE Implementation

### Required Modules

- `juce::dsp::Oversampling`

### Code Pattern

```cpp
// Header - declare as member (BEFORE APVTS for initialization order)
juce::dsp::Oversampling<float> oversampling {
    2,  // numChannels
    2,  // oversampling factor (2 = 4x, 3 = 8x)
    juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple
};

// prepareToPlay
void prepareToPlay(double sampleRate, int samplesPerBlock) override
{
    oversampling.initProcessing(static_cast<size_t>(samplesPerBlock));
    oversampling.reset();

    // Report latency to host
    setLatencySamples(static_cast<int>(oversampling.getLatencyInSamples()));
}

// processBlock
void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override
{
    juce::dsp::AudioBlock<float> block(buffer);

    // Upsample
    auto oversampledBlock = oversampling.processSamplesUp(block);

    // === Do nonlinear processing here at higher sample rate ===

    // Downsample
    oversampling.processSamplesDown(block);
}
```

### Gotchas

1. **Latency reporting**: Use `setLatencySamples()` in `prepareToPlay()`, not a custom override of `getLatencySamples()` (it's not virtual in JUCE)

2. **Gain compensation**: FIR filters may add ~6dB gain. Measure with white noise and compensate if needed.

3. **DC filters after oversampling**: If using DC blocking filters, set their coefficients for the oversampled rate:
   ```cpp
   // For 4x oversampling with 20Hz HPF
   auto coeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate * 4.0, 20.0);
   ```

4. **Initialization order**: Declare oversampling member BEFORE APVTS in header to ensure proper initialization order.

## Parameters

| Factor | Multiplier | Typical Use |
|--------|------------|-------------|
| 1 | 2x | Light saturation |
| 2 | 4x | General distortion (recommended default) |
| 3 | 8x | Extreme waveshaping |
| 4 | 16x | Rarely needed, high CPU |

## CPU Considerations

- 4x oversampling roughly quadruples DSP CPU usage
- Consider offering quality settings (2x/4x/off) for CPU-constrained users
- Profile with worst-case settings (max stages, highest oversampling)
- Target <30% single core at 44.1kHz

## Filter Types

| Type | Quality | CPU | Use Case |
|------|---------|-----|----------|
| `filterHalfBandFIREquiripple` | Best | Higher | Final release |
| `filterHalfBandPolyphaseIIR` | Good | Lower | Prototyping |

## References

- JUCE DSPDemo example (WaveShaper section)
- Newfangled Audio Elevate (adaptive oversampling)
- FabFilter Pro-L 2 (4x default, 16x available)

# DC Blocking

## Overview

DC offset is a constant voltage added to an audio signal, pushing the waveform away from the zero line. Many nonlinear processes (asymmetric distortion, wave folding, rectification) introduce DC offset which must be removed to prevent speaker damage and headroom loss.

## Use Cases

- After any asymmetric distortion
- After wave folding (especially with bias)
- After rectification effects
- After any nonlinear process that may shift the signal center

## JUCE Implementation

### Required Modules

- `juce::dsp::IIR::Filter`
- `juce::dsp::IIR::Coefficients`

### Code Pattern

```cpp
// Header - one filter per channel
juce::dsp::IIR::Filter<float> dcBlockerL;
juce::dsp::IIR::Filter<float> dcBlockerR;

// prepareToPlay
void prepareToPlay(double sampleRate, int samplesPerBlock) override
{
    // 20Hz high-pass filter removes DC without audible bass loss
    auto dcBlockerCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        sampleRate,
        20.0  // Cutoff frequency in Hz
    );

    dcBlockerL.coefficients = dcBlockerCoefficients;
    dcBlockerR.coefficients = dcBlockerCoefficients;
    dcBlockerL.reset();
    dcBlockerR.reset();
}

// processBlock - sample-by-sample processing
for (int sample = 0; sample < numSamples; ++sample)
{
    leftChannel[sample] = dcBlockerL.processSample(leftChannel[sample]);
    rightChannel[sample] = dcBlockerR.processSample(rightChannel[sample]);
}
```

### With Oversampling

When using oversampling, the DC blocker should operate at the oversampled rate:

```cpp
// For 4x oversampling
auto dcBlockerCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
    sampleRate * 4.0,  // Oversampled rate!
    20.0
);
```

Place the DC blocker **after** nonlinear processing but **before** downsampling.

### Gotchas

1. **Use oversampled rate for coefficients**: If processing at 4x, multiply sampleRate by 4.

2. **One filter per channel**: IIR filters have state - can't share between channels.

3. **Call reset() in prepareToPlay**: Prevents clicks when playback starts.

4. **20Hz is safe for most content**: Lower cutoffs (10Hz) are safer but slower to settle. Higher cutoffs (30Hz+) may affect bass.

5. **Place after nonlinear processing**: DC is created by the nonlinear process, so filter after.

## Parameters

| Cutoff | Settling Time | Bass Impact | Use Case |
|--------|---------------|-------------|----------|
| 10Hz | Slow | None | Maximum safety |
| 20Hz | Medium | Negligible | Recommended default |
| 30Hz | Fast | Slight | Quick transients |
| 40Hz+ | Very fast | Audible | Not recommended |

## Alternative: First-Order vs Second-Order

The `makeHighPass` creates a second-order (12dB/oct) filter. For gentler filtering:

```cpp
// First-order (6dB/oct) - less phase shift, gentler slope
auto coeffs = juce::dsp::IIR::Coefficients<float>::makeFirstOrderHighPass(sampleRate, 20.0);
```

## CPU Considerations

- IIR filters are extremely cheap
- One multiply-add per sample per channel
- Negligible impact even at high oversampling rates

## References

- Standard practice in mastering chains
- All professional distortion plugins include DC blocking
- JUCE tutorials on IIR filter usage

# Wave Folding

## Overview

Wave folding "folds" audio back on itself when it exceeds a threshold, creating rich harmonic content. Inspired by Buchla 259 and West Coast synthesis. Great for adding grit and complexity to drums and percussion.

## Use Cases

- Drum and percussion processing
- Adding harmonics to bass
- Sound design and texture creation
- West Coast synthesis emulation

## JUCE Implementation

### Required Modules

- `juce::dsp::Oversampling` (essential - wave folding creates harmonics that alias)
- `juce::dsp::IIR::Filter` (for DC blocking)
- `juce::dsp::DryWetMixer` (for parallel mixing)

### Transfer Functions

Three character modes with different sonic qualities:

```cpp
float applyTransferFunction(float input, int mode)
{
    switch (mode)
    {
        case 0:  // Clean - Pure mathematical folding
        {
            // Piecewise linear, symmetric folding
            float output = std::abs(std::fmod(input + 1.0f, 4.0f) - 2.0f) - 1.0f;
            return output;
        }

        case 1:  // Warm - Soft-knee with tanh saturation
        {
            const float threshold = 0.9f;
            if (std::abs(input) < threshold)
            {
                return input;
            }
            else
            {
                float foldAmount = (std::abs(input) - threshold) / (1.0f - threshold);
                float saturated = threshold + (1.0f - threshold) * std::tanh(foldAmount * 3.0f);
                return std::copysign(saturated, input);
            }
        }

        case 2:  // Aggressive - Hard folding with asymmetric bias
        {
            float biased = input + 0.15f;  // Asymmetry adds even harmonics
            float output = std::abs(std::fmod(biased + 1.0f, 4.0f) - 2.0f) - 1.0f;
            output *= 1.1f;  // Slight gain boost for bite
            return output;
        }

        default:
            return input;
    }
}
```

### Multi-Stage Cascading

Cascading multiple folding stages creates exponential harmonic density:

```cpp
// Apply folding stages (1-4 cascaded applications)
for (int stage = 0; stage < numStages; ++stage)
{
    signal = applyTransferFunction(signal, mode);
}
```

### DC Offset Removal

Wave folding (especially asymmetric modes) creates DC offset. Remove with a high-pass filter:

```cpp
// In prepareToPlay - use oversampled rate!
auto dcBlockerCoefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(
    sampleRate * 4.0,  // Oversampled rate
    20.0               // 20Hz cutoff
);
dcBlockerL.coefficients = dcBlockerCoefficients;
dcBlockerR.coefficients = dcBlockerCoefficients;

// In processBlock - after folding, before downsampling
leftChannel[sample] = dcBlockerL.processSample(leftChannel[sample]);
```

### Gotchas

1. **Always use oversampling**: Wave folding creates harmonics up to infinity. 4x minimum recommended.

2. **DC blocking is mandatory for Aggressive mode**: Asymmetric transfer functions create DC offset.

3. **Pre-gain controls fold amount**: Map your "Fold" parameter to input gain before the transfer function:
   ```cpp
   float preGain = 1.0f + (foldValue * 9.0f);  // 0-100% maps to 1x-10x gain
   ```

4. **Transfer functions are stateless**: No denormal issues, but document the math clearly.

5. **4 stages at 100% = noise**: This is by design (extreme mode). Document for users.

## Parameters

| Parameter | Range | Default | Notes |
|-----------|-------|---------|-------|
| Fold | 0-100% | 25% | Maps to pre-gain (1x-10x) |
| Stages | 1-4 | 2 | More stages = more harmonics |
| Mode | Clean/Warm/Aggressive | Warm | Character selection |
| Mix | 0-100% | 50% | Dry/wet blend |

## CPU Considerations

- Transfer functions are cheap (simple math)
- Cascading adds linear CPU per stage
- Oversampling is the main CPU cost
- 4 stages + 4x oversampling should be <30% single core

## Signal Flow

```
Input → Dry Capture → Upsample 4x → Pre-Gain → Folding Stages → DC Removal → Downsample → Mix → Output
```

## References

- Buchla 259 Complex Waveform Generator (original hardware)
- Newfangled Audio Bend 259 (modern emulation)
- AudioThing Wave Box (multi-mode folder)
- FabFilter Saturn 2 (folding as distortion mode)

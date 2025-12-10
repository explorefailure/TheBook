# Bitcrusher - Creative Brief

## Overview

**Type:** Audio Effect (Lo-Fi/Distortion)
**Core Concept:** Classic bitcrusher with bit depth and sample rate reduction
**Status:** Ideated
**Created:** 2025-12-10

## Vision

A straightforward bitcrusher that delivers that crunchy, lo-fi digital degradation sound. Reduce bit depth for quantization noise and harsh stepping, reduce sample rate for aliasing artifacts. Simple, effective, and satisfying to use.

Perfect for adding grit to drums, destroying synths, or creating retro video game vibes.

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Bits | 1-24 | 8 | Bit depth reduction (fewer bits = more crunch) |
| Rate | 100-44100 Hz | 4000 | Sample rate reduction (lower = more aliasing) |
| Mix | 0-100% | 100% | Dry/wet blend |

## DSP Approach

### Bit Depth Reduction
```
// Quantize to N bits
float scale = pow(2.0f, bits - 1);
output = round(input * scale) / scale;
```

### Sample Rate Reduction
```
// Sample-and-hold at reduced rate
if (sampleCounter >= (originalRate / targetRate)) {
    heldSample = input;
    sampleCounter = 0;
}
output = heldSample;
sampleCounter++;
```

## UI Concept

**Layout:** Simple horizontal or vertical arrangement
**Visual Style:** Clean, minimal - just the controls needed
**Key Elements:**
- Bits knob/slider (1-24)
- Rate knob/slider (100-44100 Hz, logarithmic)
- Mix knob/slider (0-100%)

## Use Cases

- Adding crunch to drums and percussion
- Lo-fi hip-hop texture
- Retro video game / 8-bit sound design
- Destroying synth leads
- Creative sound mangling

## Technical Notes

- Simple DSP (no FFT, no complex algorithms)
- Low CPU usage
- Rate parameter should be logarithmic for natural feel
- Consider smoothing parameter changes to avoid clicks

## Next Steps

- [ ] Start implementation (`/plan Bitcrusher` or `/implement Bitcrusher`)

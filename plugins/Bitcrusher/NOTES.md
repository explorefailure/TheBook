# Bitcrusher Notes

## Status
- **Current Status:** 🚧 Stage 1 (Foundation + Shell Complete)
- **Version:** N/A (in development)
- **Type:** Audio Effect (Lo-Fi/Distortion)

## Lifecycle Timeline

- **2025-12-10:** Creative brief created
- **2025-12-10 (Stage 1):** Foundation complete - Build system operational, 3 parameters implemented

## Known Issues

None

## Additional Notes

### Description
Classic bitcrusher effect with bit depth and sample rate reduction. Creates crunchy lo-fi digital degradation, perfect for adding grit to drums, destroying synths, or creating retro video game vibes.

### Parameters
| ID | Name | Type | Range | Default | Description |
|----|------|------|-------|---------|-------------|
| bits | Bits | Float | 1.0-24.0 | 8.0 | Bit depth reduction |
| rate | Rate | Float | 100.0-44100.0 Hz | 4000.0 | Sample rate reduction (log scale) |
| mix | Mix | Float | 0.0-100.0% | 100.0 | Dry/wet blend |

### DSP Approach
- **Bit depth reduction:** Quantize samples to N bits
- **Sample rate reduction:** Sample-and-hold at reduced rate
- **Mix:** Dry/wet blend between processed and clean signal
- **CPU:** Low usage, simple DSP, no FFT

### Implementation Notes
- Rate parameter uses logarithmic skew centered at 2000 Hz for natural feel
- Pass-through audio in Stage 1 (DSP implementation in Stage 2)
- Stereo effect with input + output buses

### Next Steps
- Stage 2: DSP implementation (bit reduction, sample-and-hold, mixing)
- Stage 3: UI mockup design
- Stage 4: WebView UI integration

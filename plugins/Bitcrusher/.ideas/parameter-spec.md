# Bitcrusher - Parameter Specification

## Parameters

| ID | Name | Type | Range | Default | Description |
|----|------|------|-------|---------|-------------|
| bits | Bits | float | 1.0 to 24.0 | 8.0 | Bit depth reduction |
| rate | Rate | float | 100.0 to 44100.0 | 4000.0 | Sample rate reduction (Hz) |
| mix | Mix | float | 0.0 to 100.0 | 100.0 | Dry/wet blend percentage |

## Notes

- Rate parameter should use logarithmic skew for natural feel
- Bits can be integer-stepped or continuous (continuous allows subtle transitions)
- Mix at 0% = full dry (bypass), 100% = full wet

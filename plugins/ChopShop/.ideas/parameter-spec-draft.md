# ChopShop - Parameter Specification (Draft)

> Draft extracted from creative-brief.md. Full spec will be finalized before Stage 1.

## Parameters

| ID | Name | Type | Range | Default | Description |
|----|------|------|-------|---------|-------------|
| stutterRate | Stutter Rate | choice | 1/1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64 | 1/8 | Note division for stutter repeats |
| pitchShift | Pitch Shift | float | -24.0 to 24.0 | 0.0 | Semitone pitch adjustment |
| speed | Speed | float | 50.0 to 200.0 | 100.0 | Playback speed percentage |
| linkPitchSpeed | Link Pitch/Speed | bool | On/Off | Off | Couple pitch and speed like tape |
| reverse | Reverse | bool | On/Off | Off | Reverse audio chunks |
| triggerMode | Trigger Mode | choice | Always, MIDI, Threshold | Always | How stuttering is activated |
| threshold | Threshold | float | -60.0 to 0.0 | -20.0 | Sensitivity for transient trigger (dB) |
| mix | Mix | float | 0.0 to 100.0 | 100.0 | Dry/wet blend percentage |

## Effect Chain Order

| ID | Name | Type | Range | Default | Description |
|----|------|------|-------|---------|-------------|
| chainOrder | Effect Chain | string | - | "0,1,2,3" | Order of effects: 0=Stutter, 1=Pitch, 2=Speed, 3=Reverse |

## Notes

- Stutter Rate is tempo-synced (requires host BPM)
- Trigger Mode "MIDI" listens for any note-on
- Trigger Mode "Threshold" uses envelope follower on input
- Chain order stored as comma-separated indices for persistence

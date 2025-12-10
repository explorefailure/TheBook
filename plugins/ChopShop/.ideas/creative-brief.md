# ChopShop - Creative Brief

## Overview

**Type:** Audio Effect (Stutter/Glitch)
**Core Concept:** Precision stutter-glitch processor with reorderable effect chain for controlled breakbeat destruction
**Status:** Ideated
**Created:** 2025-12-05

## Vision

ChopShop is a stutter-glitch effect that channels the chaotic energy of Venetian Snares and Aphex Twin while putting full control in the producer's hands. Unlike random glitch plugins, ChopShop lets you dial in exactly the sound you want - from subtle rhythmic stutters to full breakbeat obliteration.

Built for one-shots and drum processing, it features a reorderable effect chain where stutter, pitch shift, speed manipulation, and reverse can be arranged in any order. Three trigger modes (always-on, MIDI, transient threshold) give flexibility for both studio precision and live performance mayhem.

The philosophy: maximum capability, maximum control. Every parameter of the chaos is yours to command.

## Parameters

| Parameter | Range | Default | Description |
|-----------|-------|---------|-------------|
| Stutter Rate | 1/1 - 1/64 | 1/8 | Note division for stutter repeats |
| Pitch Shift | -24 to +24 st | 0 | Semitone pitch adjustment |
| Speed | 50% - 200% | 100% | Playback speed percentage |
| Link Pitch/Speed | On/Off | Off | Couple pitch and speed like tape |
| Reverse | On/Off | Off | Reverse audio chunks |
| Trigger Mode | Always/MIDI/Threshold | Always | How stuttering is activated |
| Threshold | -60 to 0 dB | -20 dB | Sensitivity for transient trigger |
| Mix | 0% - 100% | 100% | Dry/wet blend |

## Effect Chain

Reorderable signal flow - user can drag to arrange:
1. Stutter (buffer capture and repeat)
2. Pitch Shift (semitone transposition)
3. Speed (time stretch/compress)
4. Reverse (chunk reversal)

## UI Concept

**Layout:** Compact effect panel with clear signal flow visualization
**Visual Style:** Industrial, mechanical - "chop shop" workshop aesthetic
**Key Elements:**
- Stutter rate dial (prominent, central)
- Effect chain visualization with drag-reorder
- Trigger mode selector (3-way switch)
- Link toggle for pitch/speed coupling

## Use Cases

- Chopping breakbeats into IDM stutters
- Creating glitchy drum fills and transitions
- Mangling one-shot samples for sound design
- Adding controlled chaos to percussion loops
- Live performance stutter effects via MIDI

## Inspirations

- Venetian Snares - chaotic breakbeat chopping
- Aphex Twin - precise rhythmic destruction
- dBlue Glitch - classic stutter VST
- Effectrix - sequenced glitch effects
- Hardware samplers with retrigger/stutter modes

## Technical Notes

- Buffer-based processing for stutter capture
- Tempo-synced note divisions (host BPM sync required)
- MIDI input for trigger mode
- Transient detection for threshold trigger
- Effect chain requires flexible routing architecture

## Next Steps

- [ ] Create UI mockup (`/dream ChopShop` -> option 3)
- [ ] Start implementation (`/implement ChopShop`)

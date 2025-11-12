# Changelog

All notable changes to TapeAge will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [1.0.3] - 2025-11-12

### Fixed

- **UI Hover Stability:** Eliminated subtle shaking when hovering over knobs
  - **Root Cause:** `transform: scale(1.005)` in `.knob:hover` caused sub-pixel rendering recalculation in WebView, creating imperceptible but continuous layout shifts
  - **Solution:** Replaced scale transform with border color change (matching VU meter housing behavior)
  - **Implementation:** Added `border: 2px solid rgba(212, 165, 116, 0.3)` to `.knob-body`, hover changes to `#c49564` (gold accent)
  - **User Impact:** Knobs now have stable, clean hover effect without visual artifacts
  - **Technical Details:** WebView/WebBrowserComponent struggles with micro-scale transforms (<1%), causing render loop instability
  - **Testing:** Verified across all 3 knobs (drive, age, mix)—no shake on hover, depression effect on click preserved
  - **Backward Compatibility:** Zero impact—visual-only change, no parameter or state modifications

## [1.0.2] - 2025-11-11

### Fixed

- WebView parameter knobs now correctly display saved values on plugin reload
- Fixed `valueChangedEvent` callback to read values via `getNormalisedValue()` instead of using undefined callback parameters
- Eliminated race condition where JavaScript called `getNormalisedValue()` before C++ sent initial values
- Knobs now load directly at saved positions instead of animating from zero

### Technical Details

- Root cause: JUCE's `valueChangedEvent` is a notification event that doesn't pass callback parameters
- Solution: Call `getNormalisedValue()` inside the callback to read from state object
- Documented in: troubleshooting/gui-issues/webview-parameter-undefined-event-callback-TapeAge-20251111.md
- Added to Required Reading: Pattern #15 in juce8-critical-patterns.md

## [1.0.1] - 2025-11-11

### Fixed

- Parameter state now persists correctly when closing and reopening Ableton Live
- Changed `getNumPrograms()` to return 0 to prevent Ableton from interfering with state restoration
- Fixes issue where parameters would reset to defaults on project reload

## [1.0.0] - 2025-11-11

### Added

- Initial release
- Vintage tape saturator with warm harmonic saturation
- Tape degradation effects: wow, flutter, dropout, and noise
- Three parameters: Drive (saturation), Age (degradation), Mix (dry/wet)
- Drive: 0-100%, default 50% - Controls tape saturation intensity
- Age: 0-100%, default 25% - Controls tape degradation artifacts (wow, flutter, dropout, noise)
- Mix: 0-100%, default 100% - Dry/wet blend control
- Factory presets: Unity (bypass), Subtle Warmth, Classic Tape, Worn Out, Destroyed

### Audio Processing

- Warm harmonic saturation with 2x oversampling for aliasing reduction
- Custom tanh transfer function for vintage tape character
- Wow and flutter modulation (±10 cents) using delay line with Lagrange interpolation
- Subtle tape noise generation (musical even at maximum settings)
- Random dropout simulation for authentic tape artifacts
- Equal-power dry/wet crossfade with latency compensation
- Real-time safe processing (no allocations in audio callback)

### User Interface

- WebView-based vintage hardware aesthetic
- Vintage VU meter with peak metering and ballistic animation (30 FPS C++ + 60 FPS JS)
- Three brass rotary knobs with gold indicators (horizontal layout)
- Creamy beige textured background with burnt orange/brown accents
- Clean sans-serif all-caps typography
- 500x450px window size

### Validation

- Passed pluginval strict validation (strictness level 10)
- VST3: All tests passed
- AU: All tests passed + auval validation
- Tested in Ableton Live and Standalone mode
- Thread safety verified
- State save/restore verified
- Parameter automation verified

### Formats

- VST3
- AU (Audio Unit)
- Standalone application

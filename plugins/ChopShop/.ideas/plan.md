# ChopShop - Implementation Plan

**Date:** 2025-12-09
**Complexity Score:** 5.0 (Complex)
**Strategy:** Phase-based implementation

---

## Complexity Factors

- **Parameters:** 8 parameters (8/5 = 1.6 points, capped at 2.0) = 1.6
- **Algorithms:** 5 complex DSP components = 5
  - Stutter/Buffer Capture Engine (custom circular buffer with tempo sync)
  - Pitch Shifting Engine (custom granular synthesis)
  - Speed/Time Manipulation Engine (custom time-stretching)
  - Transient Detection System (custom dual envelope follower)
  - Reorderable Effect Chain (custom dynamic routing)
- **Features:** 1 point
  - External MIDI control (+1)
- **Total:** 1.6 + 5 + 1 = 7.6 (capped at 5.0)

---

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 1: Foundation ← Next
- Stage 2: Shell
- Stage 3: DSP [phased - 3 phases]
- Stage 3: GUI [phased - 2 phases]
- Stage 3: Validation

---

## Complex Implementation (Score = 5.0)

### Stage 3: DSP Phases

#### Phase 4.1: Core Stutter Processing

**Goal:** Implement basic stutter engine with tempo sync and dry/wet mixing

**Components:**
- Stutter/Buffer Capture Engine
  - Circular buffer allocation
  - Tempo sync via AudioPlayHead
  - Buffer segment capture and repeat
  - Crossfade at loop boundaries
- Dry/Wet Mixer
  - juce::dsp::DryWetMixer integration
  - Latency compensation
- Trigger Mode: Always (continuous stutter)
  - MIDI and threshold modes deferred to Phase 4.2

**Test Criteria:**
- [ ] Plugin loads in DAW without crashes
- [ ] Audio passes through (dry signal audible at mix=0%)
- [ ] Stutter repeats captured buffer segment
- [ ] Stutter rate parameter changes buffer length correctly (1/1 to 1/64)
- [ ] Tempo sync works with host BPM (test at 80, 120, 140 BPM)
- [ ] Crossfade prevents clicks at loop boundaries
- [ ] Mix parameter blends dry/wet correctly (0-100%)
- [ ] No artifacts or discontinuities in stutter repeats

---

#### Phase 4.2: Trigger Modes and Basic Effects

**Goal:** Implement MIDI trigger, threshold trigger, and basic reverse effect

**Components:**
- MIDI Trigger System
  - MIDI input handling
  - Note-on detection (any note triggers)
  - Buffer capture on trigger
- Transient Detection System
  - Dual envelope follower (fast/slow)
  - Threshold parameter (dB)
  - Hysteresis to prevent re-triggering
- Reverse Processing
  - In-place buffer reversal
  - Crossfade when toggling reverse on/off
- Trigger Mode parameter (Always/MIDI/Threshold)

**Test Criteria:**
- [ ] MIDI mode triggers stutter on note-on (any MIDI note)
- [ ] Threshold mode triggers stutter on transient peaks
- [ ] Threshold parameter adjusts sensitivity (-60dB to 0dB)
- [ ] Transient detection works at various signal levels (threshold-independent)
- [ ] Reverse parameter reverses buffer playback
- [ ] Reverse toggle crossfades smoothly (no clicks)
- [ ] Trigger modes are mutually exclusive (switching modes works correctly)

---

#### Phase 4.3: Pitch Shift, Speed, and Reorderable Chain

**Goal:** Implement granular pitch shifting, time-stretching, and dynamic effect chain routing

**Components:**
- Pitch Shifting Engine
  - Granular synthesis with Hann window
  - 50ms grain size, 75% overlap
  - Pitch ratio calculation (2^(semitones/12))
  - Overlap-add synthesis
- Speed/Time Manipulation Engine
  - Independent mode: Time-stretching (granular with grain size scaling)
  - Linked mode: Varispeed (simple resampling, pitch changes with speed)
  - linkPitchSpeed parameter toggles modes
- Reorderable Effect Chain
  - chainOrder parameter parsing (e.g., "0,1,2,3")
  - Dynamic signal routing based on effect indices
  - Effect indices: 0=Stutter, 1=Pitch, 2=Speed, 3=Reverse

**Test Criteria:**
- [ ] Pitch shift parameter transposes audio (-24 to +24 semitones)
- [ ] Granular artifacts are acceptable for glitch effect
- [ ] Speed parameter changes playback speed (50% to 200%)
- [ ] Independent mode maintains pitch when speed changes
- [ ] Linked mode couples pitch and speed (tape-style varispeed)
- [ ] linkPitchSpeed toggle switches between modes
- [ ] chainOrder parameter controls effect routing order
- [ ] All 24 permutations of effect order work without crashes
- [ ] CPU usage is acceptable (<80% single core)
- [ ] Latency compensation works (getLatencySamples() reports correctly)

---

### Stage 3: GUI Phases

#### Phase 5.1: Headless Parameter Controls

**Goal:** Minimal headless UI with basic parameter controls for testing

**Components:**
- Basic PluginEditor with minimal UI
- APVTS parameter attachments for all parameters
- No designed mockup (user requested headless/minimal UI)
- Parameter labels and sliders for:
  - Stutter Rate (dropdown: 1/1, 1/2, 1/4, 1/8, 1/16, 1/32, 1/64)
  - Pitch Shift (slider: -24 to +24 semitones)
  - Speed (slider: 50% to 200%)
  - Link Pitch/Speed (toggle button)
  - Reverse (toggle button)
  - Trigger Mode (dropdown: Always, MIDI, Threshold)
  - Threshold (slider: -60dB to 0dB)
  - Mix (slider: 0% to 100%)

**Test Criteria:**
- [ ] Plugin editor window opens
- [ ] All parameters are visible and labeled
- [ ] Parameter controls update DSP when changed
- [ ] Host automation updates controls
- [ ] Preset changes update all controls

---

#### Phase 5.2: Chain Order UI (Optional Enhancement)

**Goal:** Add UI for reorderable effect chain (if time permits)

**Components:**
- Visual representation of effect chain order
- Drag-and-drop reordering (or button-based reordering)
- chainOrder parameter updates on UI change
- Effect labels: Stutter, Pitch, Speed, Reverse

**Test Criteria:**
- [ ] Effect chain order visualization matches chainOrder parameter
- [ ] Reordering UI updates chainOrder string
- [ ] Parameter changes are reflected in DSP immediately
- [ ] Visual feedback for current effect order

**Note:** This phase is optional. If time is limited, chainOrder can be controlled via parameter automation or DAW preset editing.

---

### Implementation Flow

- Stage 1: Foundation - project structure
- Stage 2: Shell - APVTS parameters
- Stage 3: DSP - 3 phases
  - Phase 4.1: Core Stutter Processing
  - Phase 4.2: Trigger Modes and Basic Effects
  - Phase 4.3: Pitch Shift, Speed, and Reorderable Chain
- Stage 3: GUI - 2 phases
  - Phase 5.1: Headless Parameter Controls
  - Phase 5.2: Chain Order UI (optional)
- Stage 3: Validation - presets, pluginval, changelog

---

## Implementation Notes

### Thread Safety
- All parameter reads use atomic `getRawParameterValue()->load()`
- Buffer capture and playback in audio thread (no allocations)
- Trigger detection (MIDI, threshold) runs in audio thread (real-time safe)
- chainOrder string parsing cached (not parsed every processBlock)
- No locks in audio thread (APVTS handles atomic parameter updates)

### Performance
- **Stutter engine:** ~10% CPU (buffer management, crossfade)
- **Granular pitch shift:** ~30-40% CPU (4x overlap, windowing, resampling)
- **Time-stretching (independent mode):** ~20-30% CPU (granular synthesis)
- **Linked mode (varispeed):** ~5% CPU (simple resampling)
- **Transient detection:** ~5% CPU (dual envelope followers)
- **Total estimated (worst case):** ~80% single core at 48kHz (all effects active, independent mode)
- **Total estimated (best case):** ~30% single core (linked mode, no transient detection)

**Optimization opportunities:**
- Use SIMD for granular synthesis overlap-add
- Cache grain windows (Hann window is constant)
- Reduce overlap to 50% if CPU too high (2x instead of 4x)

### Latency
- **Stutter buffer:** Variable latency based on stutterRate (e.g., 1/8 note at 120 BPM = 250ms)
- **Granular pitch shift:** 50ms grain size = 50ms latency
- **Time-stretching:** 50ms grain size = 50ms latency
- **Total latency (worst case):** ~300ms (stutter + pitch + speed)
- Report total via `getLatencySamples()` for host compensation
- DryWetMixer handles latency compensation for dry signal automatically

### Denormal Protection
- Use `juce::ScopedNoDenormals` in processBlock()
- Envelope followers use floor value (`max(env, 1e-6)`) to prevent denormals
- Granular synthesis uses Hann window (never reaches exact zero)

### Known Challenges

**1. Granular pitch shift artifacts:**
- Graininess audible at large pitch shifts (>12 semitones)
- Mitigation: Tune grain size (test 30ms, 50ms, 70ms) and overlap (test 50%, 75%, 87.5%)
- Fallback: Implement phase vocoder if artifacts unacceptable
- Alternative: Use Rubber Band Library (external dependency)

**2. Tempo sync reliability:**
- Not all hosts provide reliable BPM (check AudioPlayHead::getPosition())
- BPM changes during playback (tempo automation)
- Mitigation: Implement BPM fallback (default to 120 BPM if host doesn't provide)
- Test with: Logic Pro, Ableton Live, FL Studio, Reaper

**3. Reorderable chain complexity:**
- Some effect orders may produce unexpected artifacts
- Reverse before pitch shift: Different artifacts than pitch before reverse
- Mitigation: Test all 24 permutations, document best orders in presets
- Fallback: Fixed chain order if routing complexity too high

**4. Buffer allocation size:**
- Worst case: 4 bars at 40 BPM @ 192kHz = ~460k samples (~1.8MB)
- Allocation in prepareToPlay (safe, not in audio thread)
- Mitigation: Check available memory, limit buffer size if needed

**5. MIDI input routing:**
- Requires `NEEDS_MIDI_INPUT TRUE` in CMakeLists.txt
- Some hosts require explicit MIDI routing to plugin
- Mitigation: Document MIDI setup in user manual
- Test MIDI trigger in multiple hosts

---

## References

- Creative brief: `plugins/ChopShop/.ideas/creative-brief.md`
- Parameter spec: `plugins/ChopShop/.ideas/parameter-spec-draft.md` (draft from brief)
- DSP architecture: `plugins/ChopShop/.ideas/architecture.md`
- UI mockup: N/A (headless/minimal UI requested)

**Similar plugins for reference:**
- **GainKnob** - Reference for basic WebView parameter binding (if UI implemented)
- **TapeAge** - Reference for dry/wet mixing and latency compensation
- **FlutterVerb** - Reference for MIDI input handling
- **LushPad** - Reference for MIDI trigger and synthesis routing

**Professional plugins researched:**
- dBlue Glitch 2 - Step-sequenced stutter effect
- Effectrix 2 - Multi-effect sequencer with grain engine
- Stutter Edit 2 - MIDI-triggered buffer manipulation
- DFX Buffer Override - Simple buffer-based stutter

---

## Phase Durations (Estimated)

**Stage 1: Foundation + Shell**
- Foundation: 30 minutes (CMakeLists.txt, project structure)
- Shell: 45 minutes (9 APVTS parameters including chainOrder)
- **Total:** 1 hour 15 minutes

**Stage 3: DSP**
- Phase 4.1 (Core Stutter): 3 hours (circular buffer, tempo sync, crossfade)
- Phase 4.2 (Trigger Modes): 2 hours (MIDI, transient detection, reverse)
- Phase 4.3 (Pitch/Speed/Chain): 5 hours (granular synthesis, time-stretching, routing)
- **Total:** 10 hours

**Stage 3: GUI**
- Phase 5.1 (Headless UI): 1 hour (basic parameter controls)
- Phase 5.2 (Chain Order UI): 2 hours (optional, if implemented)
- **Total:** 1-3 hours

**Stage 3: Validation**
- Presets: 30 minutes (5-10 presets showcasing effect orders)
- Pluginval: 30 minutes (test and fix validation errors)
- Changelog: 15 minutes
- **Total:** 1 hour 15 minutes

**Grand Total:** 13.5 - 15.5 hours (depending on optional Chain Order UI)

---

## Success Criteria

**Phase 4.1 succeeds when:**
- Stutter engine captures and repeats buffer segments
- Tempo sync works with host BPM (tested at multiple tempos)
- Crossfade prevents clicks at loop boundaries
- Mix parameter blends dry/wet correctly
- No crashes or audio glitches

**Phase 4.2 succeeds when:**
- MIDI trigger captures buffer on note-on
- Threshold trigger detects transients at all signal levels
- Reverse parameter works without clicks
- All three trigger modes are mutually exclusive

**Phase 4.3 succeeds when:**
- Pitch shift transposes audio across full range (-24 to +24 semitones)
- Speed changes playback rate correctly (50% to 200%)
- Linked/independent modes work as specified
- chainOrder parameter controls effect routing
- CPU usage is acceptable (<80% single core)
- Latency compensation reports correctly

**Phase 5.1 succeeds when:**
- All parameters are controllable via UI
- Host automation works
- Presets update UI correctly

**Project succeeds when:**
- All phases complete successfully
- Pluginval passes
- 5-10 presets showcase plugin capabilities
- Changelog documents all features

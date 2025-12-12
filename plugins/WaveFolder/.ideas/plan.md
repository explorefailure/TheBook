# WaveFolder - Implementation Plan

**Date:** 2025-12-12
**Complexity Score:** 1.8 (Simple)
**Strategy:** Single-pass implementation

---

## Complexity Factors

- **Parameters:** 4 parameters (4/5 = 0.8 points) = 0.8
- **Algorithms:** 4 DSP components = 1.0
  - Wave folding engine (WaveShaper with 3 modes)
  - Oversampling system
  - DC offset removal filter
  - Dry/wet mixer
- **Features:** 0 points
  - No feedback loops
  - No FFT/frequency domain processing
  - No multiband processing
  - No modulation systems
  - No external MIDI control
- **Total:** 1.8 (well below 3.0 threshold)

---

## Stages

- Stage 0: Research ✓
- Stage 1: Planning ✓
- Stage 1: Foundation ← Next
- Stage 2: Shell
- Stage 3: DSP
- Stage 3: GUI
- Stage 3: Validation

---

## Simple Implementation (Score ≤ 2.0)

### Implementation Flow

- Stage 1: Foundation - project structure
- Stage 2: Shell - APVTS parameters
- Stage 3: DSP - single pass
- Stage 3: GUI - single pass
- Stage 3: Validation - presets, pluginval, changelog

### Implementation Notes

**DSP Approach:**

This is a straightforward distortion effect with well-established algorithms.

**Single-pass DSP implementation:**
1. Set up JUCE dsp::Oversampling (4x, filterHalfBandFIREquiripple)
2. Implement three transfer functions as lambdas (Clean, Warm, Aggressive)
3. Create dsp::WaveShaper instances for each mode
4. Implement multi-stage cascading (loop 1-4 times based on STAGES parameter)
5. Add DC offset removal (first-order high-pass filter at 20Hz)
6. Configure dsp::DryWetMixer for latency-compensated mixing
7. Connect parameters to DSP components (FOLD → pre-gain, MODE → transfer function selection, STAGES → loop count, MIX → dry/wet proportion)

**Processing chain order:**
- Capture dry → Upsample → Pre-gain → Stage loop → DC removal → Downsample → Dry/wet mix

**Key DSP considerations:**
- Oversampling gain compensation (~6dB boost from FIR filters, measure and compensate)
- Transfer function testing with sine sweep + spectrum analyzer
- CPU profiling with 4 stages + 4x oversampling (target <30% single core)

**GUI Approach:**

Simple WebView UI with four controls:
- FOLD knob (0-100%)
- STAGES selector (1, 2, 3, 4)
- MODE switch (Clean, Warm, Aggressive)
- MIX knob (0-100%)

**Single-pass GUI implementation:**
1. Copy mockup HTML to Source/ui/public/index.html (when mockup is created)
2. Set up WebView with resource provider
3. Bind parameters using WebSliderRelay and WebSliderParameterAttachment
4. Implement MODE switcher (button group or dropdown)
5. Implement STAGES selector (button group or dropdown)
6. Test parameter automation from host

**Key GUI considerations:**
- MODE and STAGES are Choice parameters (not continuous)
- Use button groups or dropdown selectors (not knobs)
- Visual feedback for selected mode/stage count
- Clean, minimal design matching creative brief

**Key Considerations:**

- **Oversampling is the highest risk component:**
  - Test with white noise to measure gain change
  - Apply compensating attenuation if needed (-6dB typical)
  - Benchmark CPU with different oversampling factors (consider 2x/4x/off modes)

- **Transfer function validation:**
  - Test with sine sweep (20Hz-20kHz) → spectrum analyzer
  - Verify harmonic content matches expectations
  - Check for aliasing at high FOLD amounts

- **DC offset in Aggressive mode:**
  - Measure DC offset with DC meter plugin
  - Verify 20Hz HPF removes DC without audible bass loss
  - Consider adjustable HPF cutoff if 20Hz too aggressive

- **Multi-stage behavior:**
  - 4 stages at 100% FOLD may produce noise-like character (by design)
  - Document as "extreme mode" in user manual
  - Typical use case: 1-2 stages for drums

- **Potential gotchas:**
  - JUCE Oversampling gain change (measure and compensate)
  - dsp::WaveShaper requires std::function or lambda (not raw function pointer)
  - DC removal must be after folding but before downsampling
  - DryWetMixer handles latency compensation automatically (don't manually compensate)

---

## References

- Creative brief: `plugins/WaveFolder/.ideas/creative-brief.md`
- Parameter spec: `plugins/WaveFolder/.ideas/parameter-spec.md`
- DSP architecture: `plugins/WaveFolder/.ideas/architecture.md`
- UI mockup: TBD (run `/dream WaveFolder` option 3)

**Similar plugins for reference:**
- **GainKnob** - Simple parameter structure, WebView basics
- **TapeMachine** - Distortion/saturation DSP, dry/wet mixing
- **TapeAge** - Multiple processing modes, mode switching in GUI

**JUCE examples:**
- JUCE DSPDemo (WaveShaper example)
- JUCE Oversampling examples in dsp module

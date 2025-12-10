#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class ChopShopAudioProcessor : public juce::AudioProcessor
{
public:
    ChopShopAudioProcessor();
    ~ChopShopAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "ChopShop"; }
    bool acceptsMidi() const override { return true; }   // MIDI input for trigger mode
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // Public APVTS for editor access
    juce::AudioProcessorValueTreeState parameters;

private:
    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // Custom state for chain order (not in APVTS)
    juce::ValueTree customState;

    // DSP Components (Phase 2.1: Core Stutter Processing)
    juce::dsp::ProcessSpec spec;
    juce::dsp::DryWetMixer<float> dryWetMixer;

    // Stutter buffer (circular buffer for audio capture)
    juce::AudioBuffer<float> stutterBuffer;
    int stutterBufferSize = 0;
    int capturePosition = 0;      // Write position in circular buffer
    int playbackPosition = 0;     // Read position in circular buffer
    int segmentLength = 0;        // Current segment length in samples

    // Tempo sync state
    double currentBPM = 120.0;    // Cached BPM from host
    double currentSampleRate = 44100.0;

    // Phase 2.2: Trigger Modes + Effects
    // Envelope followers for transient detection (stereo)
    float fastEnvelope[2] = {0.0f, 0.0f};
    float slowEnvelope[2] = {0.0f, 0.0f};
    float fastAttackCoeff = 0.0f;
    float fastReleaseCoeff = 0.0f;
    float slowAttackCoeff = 0.0f;
    float slowReleaseCoeff = 0.0f;

    // Trigger state
    bool triggered = false;       // Transient trigger state (for hysteresis)
    bool lastReverse = false;     // Track reverse state changes for crossfade

    // Phase 2.3: Pitch Shift, Speed, Effect Chain
    std::array<int, 4> effectOrder;  // Cached chain order
    juce::AudioBuffer<float> processBuffer;  // Intermediate processing buffer
    float pitchReadPosition = 0.0f;  // Fractional read position for pitch shift

    // Helper methods
    float getDivisionValue(int stutterRateIndex) const;
    void captureAudio(juce::AudioBuffer<float>& buffer);
    void playbackStutter(juce::AudioBuffer<float>& buffer, bool reverseEnabled);
    void triggerCapture();  // Phase 2.2: Reset stutter buffer capture position

    // Phase 2.3: Effect processing methods
    void parseChainOrder();
    void applyStutter(juce::AudioBuffer<float>& buffer);
    void applyPitchShift(juce::AudioBuffer<float>& buffer, float pitchSemitones, float speedPercent, bool linked);
    void applyReverse(juce::AudioBuffer<float>& buffer);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ChopShopAudioProcessor)
};

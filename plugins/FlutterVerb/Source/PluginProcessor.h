#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class FlutterVerbAudioProcessor : public juce::AudioProcessor
{
public:
    FlutterVerbAudioProcessor();
    ~FlutterVerbAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "FlutterVerb"; }
    bool acceptsMidi() const override { return false; }
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

    // Public accessor for APVTS (required for WebView parameter binding)
    juce::AudioProcessorValueTreeState& getAPVTS() { return parameters; }

private:
    // DSP Components (declare BEFORE parameters for initialization order)
    juce::dsp::ProcessSpec spec;

    // Phase 4.1: Core Reverb Processing
    juce::dsp::Reverb reverb;
    juce::dsp::DryWetMixer<float> dryWetMixer;

    // Phase 4.2: Modulation System
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Lagrange3rd> modulationDelay { 9600 }; // 200ms at 48kHz
    std::vector<float> wowPhase;    // Per-channel wow LFO phase (0-2π)
    std::vector<float> flutterPhase; // Per-channel flutter LFO phase (0-2π)
    double currentSampleRate = 44100.0; // Store sample rate for LFO calculations

    // Phase 4.3: Saturation and Filter
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>> toneFilter;
    enum class FilterType { None, LowPass, HighPass };
    FilterType currentFilterType = FilterType::None;

    // APVTS comes AFTER DSP components
    juce::AudioProcessorValueTreeState parameters;

    // Parameter layout creation
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(FlutterVerbAudioProcessor)
};

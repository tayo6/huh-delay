#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

/**
    HuhDelayAudioProcessor
    -----------------------
    Mono delay plugin processor.

    NOTE: Per the current task scope, this class intentionally does NOT
    implement delay DSP yet. It exists to:
      1) Own an AudioProcessorValueTreeState (APVTS) so every knob / button
         drawn in HuhLookAndFeel + the UI/ components is backed by a real,
         host-automatable parameter (making the UI genuinely interactive,
         not just decorative).
      2) Pass audio through unmodified (processBlock is a no-op passthrough).
      3) Provide the standard JUCE AudioProcessor plumbing so the project
         builds cleanly as a VST3 in CI.

    Audio DSP (the actual delay algorithm) is left as a TODO for a later
    pass - see processBlock() in PluginProcessor.cpp.
*/
class HuhDelayAudioProcessor final : public juce::AudioProcessor
{
public:
    HuhDelayAudioProcessor();
    ~HuhDelayAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    //==============================================================================
    const juce::String getName() const override { return JucePlugin_Name; }

    bool acceptsMidi() const override  { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 2.0; }

    //==============================================================================
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Parameter IDs, shared with the editor / UI widgets so nothing has to
    // hard-code parameter name strings in more than one place.
    struct ParamIDs
    {
        static constexpr auto delayTime   = "delayTime";   // note-division choice, synced to host tempo
        static constexpr auto syncEnabled = "syncEnabled";  // bool: tempo-synced vs free time
        static constexpr auto outputGain  = "outputGain";   // dB
        static constexpr auto bypass      = "bypass";       // power button (top-right)
    };

    juce::AudioProcessorValueTreeState apvts;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HuhDelayAudioProcessor)
};

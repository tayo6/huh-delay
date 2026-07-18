#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../LookAndFeel/DesignTokens.h"

/**
    OutputMeterPanel
    ------------------
    The narrow right-hand panel: two vertical green->yellow->red gradient
    level meter bars up top, and the small "Output" gain knob underneath.

    The meter bars are currently driven by a settable 0-1 level (see
    setMeterLevels) rather than live audio, since PluginProcessor does no
    signal processing yet - this keeps the meter visually complete and
    ready to be hooked up to real peak/RMS data later without any UI
    rework.
*/
class OutputMeterPanel final : public juce::Component,
                                private juce::Timer
{
public:
    explicit OutputMeterPanel (juce::AudioProcessorValueTreeState& apvts);
    ~OutputMeterPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setMeterLevels (float leftLevel0to1, float rightLevel0to1);

private:
    void timerCallback() override;

    juce::AudioProcessorValueTreeState& state;

    juce::Slider outputKnob { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Label  outputLabel { "outputLabel", "Output" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputAttachment;

    float meterLeft  = 0.0f;
    float meterRight = 0.0f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OutputMeterPanel)
};

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../LookAndFeel/DesignTokens.h"

/**
    DelayControlPanel
    -------------------
    The left-hand panel: "HUH v0.1" title, the large delay-time rotary
    knob, its live value readout (e.g. "1 / 16 D"), and the pill-shaped
    "Sync" toggle underneath.

    The knob is bound to the delayTime choice parameter (via a
    ComboBoxAttachment-style approach implemented with a Slider driven off
    a normalised choice range, since the reference art clearly shows a
    single rotary control - not a dropdown - stepping through named
    divisions). The Sync toggle is bound directly to the syncEnabled bool
    parameter.
*/
class DelayControlPanel final : public juce::Component,
                                 private juce::Slider::Listener
{
public:
    explicit DelayControlPanel (juce::AudioProcessorValueTreeState& apvts);
    ~DelayControlPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& state;

    juce::Label titleLabel   { "title", "HUH" };
    juce::Label versionLabel { "version", "v" JucePlugin_VersionString };

    juce::Slider delayTimeKnob { juce::Slider::RotaryHorizontalVerticalDrag, juce::Slider::NoTextBox };
    juce::Label  delayValueLabel;

    juce::ToggleButton syncButton { "Sync" };

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> syncAttachment;

    void updateDelayValueLabel();

    // juce::Slider::Listener - kept separate from SliderAttachment's own
    // internal listener registration (both can coexist safely; using
    // Slider::onValueChange directly would silently replace the
    // attachment's callback and break the two-way parameter binding).
    void sliderValueChanged (juce::Slider*) override { updateDelayValueLabel(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DelayControlPanel)
};

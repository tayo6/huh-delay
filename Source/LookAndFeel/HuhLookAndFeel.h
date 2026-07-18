#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "DesignTokens.h"

/**
    HuhLookAndFeel
    ---------------
    Central LookAndFeel_V4 subclass responsible for drawing every custom
    widget in the plugin so that individual UI/ components stay free of
    raw Graphics painting code and just describe *what* they are, not
    *how* they're drawn pixel-by-pixel.

    Two rotary styles are supported, matching the reference art:
      - "Delay" knob: dark outer arc track, light-grey gradient face, single
        indicator line, no fill-from-min sweep (looks like the big knob
        under HUH title).
      - "Output" knob: white face, red partial ring + red indicator tick,
        smaller (bottom-right of the meter panel).

    A dedicated drawToggleButton override renders the pill-shaped "Sync"
    switch and the round A/B comparison + power buttons using a
    getProperties() flag to distinguish styles without needing N subclasses.
*/
class HuhLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    HuhLookAndFeel();
    ~HuhLookAndFeel() override;

    //==============================================================================
    void drawRotarySlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPosProportional, float rotaryStartAngle,
                            float rotaryEndAngle, juce::Slider&) override;

    void drawToggleButton (juce::Graphics&, juce::ToggleButton&,
                            bool shouldDrawButtonAsHighlighted,
                            bool shouldDrawButtonAsDown) override;

    // The velocity "pole" sliders in VelocityGraphPanel are fully
    // hand-painted by their parent component (stem + handle drawn directly
    // in VelocityGraphPanel::paint()); this override intentionally draws
    // nothing so the Slider only contributes drag/mouse behaviour, with no
    // default JUCE track/thumb rendered on top of the custom artwork.
    void drawLinearSlider (juce::Graphics&, int x, int y, int width, int height,
                            float sliderPos, float minSliderPos, float maxSliderPos,
                            const juce::Slider::SliderStyle, juce::Slider&) override;

    juce::Font getLabelFont (juce::Label&) override;

    // Property key used on Slider/ToggleButton instances to pick which
    // custom drawing variant to use (see .cpp for accepted values).
    static const juce::Identifier stylePropertyId;

    // Recognised values for stylePropertyId
    static const juce::String styleDelayKnob;
    static const juce::String styleOutputKnob;
    static const juce::String styleSyncPill;
    static const juce::String stylePowerButton;
    static const juce::String styleRoundIconButton; // undo/redo/help/zoom/AB pills

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HuhLookAndFeel)
};

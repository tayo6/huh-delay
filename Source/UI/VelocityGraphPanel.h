#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/DesignTokens.h"

/**
    VelocityGraphPanel
    --------------------
    The large central panel showing the "Velocity" canvas selector (with
    zoom in/out controls), horizontal gridlines, the 0 / 65 / 127 axis
    labels, and the three coloured velocity "poles" (orange, red, green)
    with their draggable circular handles.

    Interaction model: each pole's handle is a small invisible Slider
    (vertical, no text box) layered over the custom-painted stem/track, so
    users can drag it up and down exactly like the reference screenshot
    implies, without wiring this stub up to real MIDI velocity data yet
    (there is no processor-side parameter for these - they're purely a
    visual/interactive placeholder pending a future data model, tracked as
    a local 0-127 value per pole for now).
*/
class VelocityGraphPanel final : public juce::Component
{
public:
    VelocityGraphPanel();
    ~VelocityGraphPanel() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    std::function<void()> onZoomIn;
    std::function<void()> onZoomOut;

private:
    struct Pole
    {
        juce::Colour colour;
        juce::Slider slider { juce::Slider::LinearVertical, juce::Slider::NoTextBox };
    };

    juce::Label   canvasTitle { "canvasTitle", "Velocity" };

    // Text is left empty on all three; paint() hand-draws the dropdown
    // chevron and +/- glyphs so no font glyph-coverage assumptions are
    // needed and there's no risk of double-painted button text.
    juce::TextButton dropdownArrow;
    juce::TextButton zoomInButton;
    juce::TextButton zoomOutButton;

    std::array<Pole, 3> poles; // orange, red, green - left to right per reference art

    juce::Rectangle<int> graphArea; // cached in resized(), used by paint() for gridlines

    void layoutGraphArea();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VelocityGraphPanel)
};

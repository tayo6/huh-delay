#include "HuhLookAndFeel.h"

using namespace HuhTokens;

const juce::Identifier HuhLookAndFeel::stylePropertyId   { "huhStyle" };
const juce::String HuhLookAndFeel::styleDelayKnob        { "delayKnob" };
const juce::String HuhLookAndFeel::styleOutputKnob       { "outputKnob" };
const juce::String HuhLookAndFeel::styleSyncPill         { "syncPill" };
const juce::String HuhLookAndFeel::stylePowerButton      { "powerButton" };
const juce::String HuhLookAndFeel::styleRoundIconButton  { "roundIconButton" };

HuhLookAndFeel::HuhLookAndFeel()
{
    setColour (juce::ResizableWindow::backgroundColourId, Colour::outerFrame);
    setColour (juce::Slider::textBoxTextColourId, Colour::textDarkGreen);
    setColour (juce::Label::textColourId, Colour::textDarkGreen);
    setColour (juce::ComboBox::textColourId, Colour::textMutedGreen);
    setColour (juce::PopupMenu::backgroundColourId, Colour::panelGreen);
    setColour (juce::PopupMenu::textColourId, Colour::textDarkGreen);
}

HuhLookAndFeel::~HuhLookAndFeel() = default;

juce::Font HuhLookAndFeel::getLabelFont (juce::Label& label)
{
    return getUIFont (label.getFont().getHeight());
}

//==============================================================================
void HuhLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height,
                                        float sliderPosProportional, float rotaryStartAngle,
                                        float rotaryEndAngle, juce::Slider& slider)
{
    const auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (2.0f);
    const auto centre = bounds.getCentre();
    const auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f;
    const auto angle  = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

    const auto style = slider.getProperties().getWithDefault (stylePropertyId, styleDelayKnob).toString();

    if (style == styleOutputKnob)
    {
        // White face, thin dark outline
        g.setColour (Colour::knobFace);
        g.fillEllipse (bounds.reduced (radius * 0.18f));
        g.setColour (Colour::textForestGreen);
        g.drawEllipse (bounds.reduced (radius * 0.18f), 1.2f);

        // Red partial ring track, matching reference "Output" knob
        juce::Path ring;
        const float ringRadius = radius * 1.0f;
        ring.addCentredArc (centre.x, centre.y, ringRadius, ringRadius,
                             0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (Colour::outputRing.withAlpha (0.9f));
        g.strokePath (ring, juce::PathStrokeType (2.2f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Indicator tick
        juce::Path tick;
        tick.startNewSubPath (0.0f, -radius * 0.15f);
        tick.lineToXY (0.0f, -radius * 0.75f);
        tick.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
        g.setColour (Colour::textForestGreen);
        g.strokePath (tick, juce::PathStrokeType (2.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    else // styleDelayKnob (default)
    {
        // Outer dark arc track (the big rounded-cap arc behind the knob face)
        juce::Path track;
        const float trackRadius = radius * 1.02f;
        track.addCentredArc (centre.x, centre.y, trackRadius, trackRadius,
                              0.0f, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour (Colour::outerFrame);
        g.strokePath (track, juce::PathStrokeType (radius * 0.14f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // Knob face: light radial-ish gradient (approximated with a linear
        // gradient across the face, matching the SVG's grey->white sweep)
        juce::ColourGradient grad (Colour::knobFaceShadow, bounds.getX(), bounds.getY(),
                                    Colour::knobFace, bounds.getRight(), bounds.getBottom(), false);
        g.setGradientFill (grad);
        const auto faceBounds = bounds.reduced (radius * 0.15f);
        g.fillEllipse (faceBounds);
        g.setColour (Colour::outerFrame);
        g.drawEllipse (faceBounds, 1.0f);

        // Indicator line across the face (matches the single horizontal-ish
        // dash seen in the reference art, rotating with value)
        juce::Path indicator;
        const float lineLen = faceBounds.getWidth() * 0.34f;
        indicator.startNewSubPath (-lineLen, 0.0f);
        indicator.lineToXY (lineLen, 0.0f);
        indicator.applyTransform (juce::AffineTransform::rotation (angle).translated (centre));
        g.setColour (Colour::outerFrame);
        g.strokePath (indicator, juce::PathStrokeType (radius * 0.11f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
}

//==============================================================================
void HuhLookAndFeel::drawLinearSlider (juce::Graphics&, int, int, int, int,
                                        float, float, float,
                                        const juce::Slider::SliderStyle, juce::Slider&)
{
    // Intentionally blank - see header comment. VelocityGraphPanel draws
    // the stem + handle for each pole itself.
}

//==============================================================================
void HuhLookAndFeel::drawToggleButton (juce::Graphics& g, juce::ToggleButton& button,
                                        bool shouldDrawButtonAsHighlighted, bool /*shouldDrawButtonAsDown*/)
{
    const auto style = button.getProperties().getWithDefault (stylePropertyId, styleSyncPill).toString();
    auto bounds = button.getLocalBounds().toFloat();

    if (style == styleSyncPill)
    {
        // Pill-shaped background
        const float cornerRadius = bounds.getHeight() * 0.5f;
        g.setColour (Colour::syncPillFill.withAlpha (shouldDrawButtonAsHighlighted ? 0.85f : 1.0f));
        g.fillRoundedRectangle (bounds, cornerRadius);

        // Small square ring indicator on the left, "Sync" label to the right
        const float ringSize = bounds.getHeight() * 0.42f;
        auto ringBounds = bounds.withSizeKeepingCentre (ringSize, ringSize)
                                 .withX (bounds.getX() + bounds.getHeight() * 0.29f);

        g.setColour (button.getToggleState() ? Colour::white : Colour::syncPillRing);
        g.drawRoundedRectangle (ringBounds, ringSize * 0.22f, 1.4f);
        if (button.getToggleState())
            g.fillRoundedRectangle (ringBounds.reduced (ringSize * 0.22f), ringSize * 0.12f);

        g.setColour (Colour::white);
        g.setFont (getUIFont (bounds.getHeight() * 0.42f, true));
        auto textArea = bounds.withTrimmedLeft (bounds.getHeight() * 0.85f);
        g.drawText (button.getButtonText(), textArea, juce::Justification::centredLeft);
    }
    else if (style == stylePowerButton)
    {
        // NOTE: this button is attached to the "bypass" parameter, where
        // toggleState == true means BYPASSED. We invert here so the glyph
        // reads as lit/green when the plugin is actively processing
        // (bypass == false), which is the intuitive reading of a power icon.
        const bool isActive = ! button.getToggleState();

        auto circleBounds = bounds.reduced (1.0f);
        g.setColour (isActive ? Colour::meterGreen.withAlpha (0.85f) : Colour::pillBackground);
        g.drawEllipse (circleBounds, 1.6f);

        // Power glyph: vertical tick + partial ring gap at top
        juce::Path glyph;
        const auto c = circleBounds.getCentre();
        const auto r = circleBounds.getWidth() * 0.5f;
        glyph.addCentredArc (c.x, c.y, r * 0.62f, r * 0.62f,
                              0.0f, juce::MathConstants<float>::pi * 0.28f,
                              juce::MathConstants<float>::twoPi - juce::MathConstants<float>::pi * 0.28f,
                              true);
        g.setColour (isActive ? Colour::meterGreen : Colour::pillBackground);
        g.strokePath (glyph, juce::PathStrokeType (1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        g.drawLine (c.x, circleBounds.getY() + r * 0.18f, c.x, c.y, 1.6f);
    }
    else if (style == styleRoundIconButton)
    {
        g.setColour (Colour::pillBackground.withAlpha (shouldDrawButtonAsHighlighted ? 0.8f : 1.0f));
        g.fillRoundedRectangle (bounds, bounds.getHeight() * 0.5f);
    }
    else
    {
        LookAndFeel_V4::drawToggleButton (g, button, shouldDrawButtonAsHighlighted, false);
    }
}

#include "HeaderBar.h"
#include "PresetBar.h"
#include "../LookAndFeel/HuhLookAndFeel.h"
#include "../PluginProcessor.h"

using namespace HuhTokens;

// All of prevButton / nextButton / undoButton / redoButton / buttonA /
// buttonB / helpButton are deliberately kept as *visually invisible*
// juce::TextButtons: they exist purely to own their Component bounds and
// handle mouse clicks. Every pixel you actually see for these controls is
// hand-painted in HeaderBar::paint(), which runs as this component's
// background and therefore draws *underneath* the (invisible) buttons.
// This avoids double-painting that would otherwise happen if both this
// class's paint() and each button's own LookAndFeel-driven paintButton()
// tried to render the same glyph.
namespace
{
    void makeInvisibleClickTarget (juce::TextButton& b)
    {
        b.setButtonText ({});
        b.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        b.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        b.setColour (juce::TextButton::textColourOffId, juce::Colours::transparentBlack);
        b.setColour (juce::TextButton::textColourOnId, juce::Colours::transparentBlack);
    }
}

HeaderBar::HeaderBar (juce::AudioProcessorValueTreeState& apvts) : state (apvts)
{
    presetBar = std::make_unique<PresetBar>();
    addAndMakeVisible (*presetBar);
    presetBar->onStep = [this] (int dir) { if (onPresetStep) onPresetStep (dir); };

    for (auto* b : { &prevButton, &nextButton, &undoButton, &redoButton, &buttonA, &buttonB, &helpButton })
    {
        makeInvisibleClickTarget (*b);
        addAndMakeVisible (*b);
    }

    prevButton.onClick  = [this] { if (onPresetStep) onPresetStep (-1); };
    nextButton.onClick  = [this] { if (onPresetStep) onPresetStep (+1); };
    undoButton.onClick  = [this] { if (onUndo) onUndo(); };
    redoButton.onClick  = [this] { if (onRedo) onRedo(); };
    helpButton.onClick  = [this] { if (onHelp) onHelp(); };

    buttonA.setClickingTogglesState (true);
    buttonB.setClickingTogglesState (true);
    buttonA.setRadioGroupId (0xA0B0, juce::dontSendNotification);
    buttonB.setRadioGroupId (0xA0B0, juce::dontSendNotification);
    buttonA.setToggleState (true, juce::dontSendNotification);
    buttonA.onClick = [this] { repaint(); };
    buttonB.onClick = [this] { repaint(); };

    powerButton.getProperties().set (HuhLookAndFeel::stylePropertyId, HuhLookAndFeel::stylePowerButton);
    addAndMakeVisible (powerButton);
    powerAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        state, HuhDelayAudioProcessor::ParamIDs::bypass, powerButton);
}

HeaderBar::~HeaderBar() = default;

void HeaderBar::resized()
{
    auto bounds = getLocalBounds().reduced (10, 6);

    prevButton.setBounds (bounds.removeFromLeft (18));
    bounds.removeFromLeft (4);
    nextButton.setBounds (bounds.removeFromLeft (18));
    bounds.removeFromLeft (10);

    powerButton.setBounds (bounds.removeFromRight (26));
    bounds.removeFromRight (10);
    helpButton.setBounds (bounds.removeFromRight (26));
    bounds.removeFromRight (12);

    auto abArea = bounds.removeFromRight (48);
    buttonB.setBounds (abArea.removeFromRight (22));
    buttonA.setBounds (abArea.removeFromRight (22));
    bounds.removeFromRight (12);

    auto redoArea = bounds.removeFromRight (26);
    redoButton.setBounds (redoArea);
    bounds.removeFromRight (4);
    auto undoArea = bounds.removeFromRight (26);
    undoButton.setBounds (undoArea);
    bounds.removeFromRight (12);

    presetBar->setBounds (bounds.withSizeKeepingCentre (juce::jmin (bounds.getWidth(), 260), 24));
}

void HeaderBar::drawChevron (juce::Graphics& g, juce::Rectangle<float> bounds, bool pointingRight)
{
    juce::Path p;
    const float w = bounds.getWidth() * 0.4f;
    const float h = bounds.getHeight() * 0.35f;
    auto c = bounds.getCentre();
    if (pointingRight)
    {
        p.startNewSubPath (c.x - w * 0.5f, c.y - h);
        p.lineTo (c.x + w * 0.5f, c.y);
        p.lineTo (c.x - w * 0.5f, c.y + h);
    }
    else
    {
        p.startNewSubPath (c.x + w * 0.5f, c.y - h);
        p.lineTo (c.x - w * 0.5f, c.y);
        p.lineTo (c.x + w * 0.5f, c.y + h);
    }
    g.setColour (Colour::pillBackground);
    g.strokePath (p, juce::PathStrokeType (1.6f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
}

void HeaderBar::paint (juce::Graphics& g)
{
    g.fillAll (Colour::panelGreenDark);

    drawChevron (g, prevButton.getBounds().toFloat(), false);
    drawChevron (g, nextButton.getBounds().toFloat(), true);

    // Undo / redo curved-arrow glyphs
    auto drawCurvedArrow = [&g] (juce::Rectangle<float> b, bool mirrored)
    {
        auto area = b.reduced (b.getWidth() * 0.22f);
        juce::Path arc;
        arc.addCentredArc (area.getCentreX(), area.getCentreY(), area.getWidth() * 0.5f, area.getHeight() * 0.5f,
                            0.0f,
                            mirrored ? juce::MathConstants<float>::pi * 1.5f : juce::MathConstants<float>::pi * 1.0f,
                            mirrored ? juce::MathConstants<float>::pi * 2.35f : juce::MathConstants<float>::pi * 1.85f,
                            true);
        g.setColour (Colour::textForestGreen);
        g.strokePath (arc, juce::PathStrokeType (1.3f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        // arrowhead
        auto tip = mirrored ? area.getRelativePoint (1.0f, 0.12f) : area.getRelativePoint (0.0f, 0.12f);
        juce::Path head;
        const float s = 2.6f;
        head.addTriangle (tip.x - s, tip.y - s, tip.x + s, tip.y - s, tip.x, tip.y + s);
        g.fillPath (head);
    };
    drawCurvedArrow (undoButton.getBounds().toFloat(), false);
    drawCurvedArrow (redoButton.getBounds().toFloat(), true);

    // A / B labels drawn as hand-painted pills (buttons underneath are
    // invisible click targets only - see makeInvisibleClickTarget above)
    auto drawAB = [&g] (juce::Rectangle<int> b, const juce::String& text, bool active)
    {
        g.setColour (active ? Colour::syncPillFill : Colour::pillBackground);
        g.fillRoundedRectangle (b.toFloat(), (float) b.getHeight() * 0.5f);
        g.setColour (active ? Colour::white : Colour::textForestGreen);
        g.setFont (getUIFont (11.0f));
        g.drawText (text, b, juce::Justification::centred);
    };
    drawAB (buttonA.getBounds(), "A", buttonA.getToggleState());
    drawAB (buttonB.getBounds(), "B", buttonB.getToggleState());

    // Help "?" pill
    g.setColour (Colour::pillBackground);
    g.fillEllipse (helpButton.getBounds().toFloat());
    g.setColour (Colour::syncPillFill);
    g.setFont (getUIFont (12.0f, true));
    g.drawText ("?", helpButton.getBounds(), juce::Justification::centred);
}

#include "PresetBar.h"

using namespace HuhTokens;

PresetBar::PresetBar()
{
    leftChevron.setButtonText ({});
    rightChevron.setButtonText ({});
    leftChevron.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    rightChevron.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    leftChevron.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    rightChevron.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);

    leftChevron.onClick  = [this] { if (onStep) onStep (-1); };
    rightChevron.onClick = [this] { if (onStep) onStep (+1); };

    addAndMakeVisible (leftChevron);
    addAndMakeVisible (rightChevron);
}

PresetBar::~PresetBar() = default;

void PresetBar::setPresetName (const juce::String& name)
{
    presetName = name;
    repaint();
}

void PresetBar::setCategoryName (const juce::String& category)
{
    categoryName = category;
    repaint();
}

void PresetBar::resized()
{
    auto bounds = getLocalBounds();
    const int chevronWidth = 16;
    leftChevron.setBounds (bounds.removeFromLeft (chevronWidth));
    rightChevron.setBounds (bounds.removeFromRight (chevronWidth));
}

void PresetBar::paint (juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Pale pill background
    g.setColour (Colour::pillBackground);
    g.fillRoundedRectangle (bounds, bounds.getHeight() * 0.28f);

    // Chevrons, drawn manually so they read crisply at small sizes
    auto drawChevron = [&g] (juce::Rectangle<float> area, bool pointingRight)
    {
        juce::Path p;
        const float w = area.getWidth() * 0.35f;
        const float h = area.getHeight() * 0.32f;
        auto c = area.getCentre();
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
        g.setColour (Colour::textForestGreen);
        g.strokePath (p, juce::PathStrokeType (1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    };

    drawChevron (leftChevron.getBounds().toFloat(), false);
    drawChevron (rightChevron.getBounds().toFloat(), true);

    // Preset name, centred in the remaining middle area
    auto middle = bounds.reduced (18.0f, 0.0f);

    g.setFont (getUIFont (13.0f, true));
    g.setColour (Colour::textForestGreen);

    // Reserve room on the right for the category badge before drawing the name
    const float badgeWidth = juce::jmax (34.0f,
        getUIFont (10.5f).getStringWidthFloat (categoryName) + 14.0f);
    auto nameArea = middle.withTrimmedRight (badgeWidth + 8.0f);
    g.drawText (presetName, nameArea, juce::Justification::centred);

    // Category badge: rounded outline pill with green text
    auto badgeArea = middle.removeFromRight (badgeWidth).reduced (0.0f, bounds.getHeight() * 0.22f);
    g.setColour (Colour::presetGreenAccent);
    g.drawRoundedRectangle (badgeArea, badgeArea.getHeight() * 0.5f, 1.0f);
    g.setFont (getUIFont (10.5f));
    g.drawText (categoryName, badgeArea, juce::Justification::centred);
}

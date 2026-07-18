#include "VelocityGraphPanel.h"

using namespace HuhTokens;

VelocityGraphPanel::VelocityGraphPanel()
{
    canvasTitle.setJustificationType (juce::Justification::centred);
    canvasTitle.setColour (juce::Label::textColourId, Colour::textDarkGreen);
    canvasTitle.setFont (getUIFont (12.0f, true));
    canvasTitle.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (canvasTitle);

    for (auto* b : { &dropdownArrow, &zoomInButton, &zoomOutButton })
    {
        // These are hand-painted in paint() (chevron / +- glyphs), so the
        // buttons themselves are kept visually transparent and only serve
        // as click targets - avoids double-drawing text/background.
        b->setButtonText ({});
        b->setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        b->setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        b->setColour (juce::TextButton::textColourOffId, juce::Colours::transparentBlack);
        b->setColour (juce::TextButton::textColourOnId, juce::Colours::transparentBlack);
        addAndMakeVisible (*b);
    }
    dropdownArrow.setInterceptsMouseClicks (false, false);

    zoomInButton.onClick  = [this] { if (onZoomIn) onZoomIn(); };
    zoomOutButton.onClick = [this] { if (onZoomOut) onZoomOut(); };

    // Reference art order left->right: orange (short), red (tall, peak),
    // green (short, near-zero)
    poles[0].colour = Colour::orangePole;
    poles[1].colour = Colour::redPole;
    poles[2].colour = Colour::greenPole;

    // Starting values chosen to roughly match the reference screenshot
    // (orange ~mid-low, red near max, green near-zero).
    std::array<double, 3> startValues { 40.0, 118.0, 8.0 };

    for (size_t i = 0; i < poles.size(); ++i)
    {
        auto& pole = poles[i];
        pole.slider.setRange (0.0, 127.0, 1.0);
        pole.slider.setValue (startValues[i], juce::dontSendNotification);
        pole.slider.setColour (juce::Slider::trackColourId, juce::Colours::transparentBlack);
        pole.slider.setColour (juce::Slider::backgroundColourId, juce::Colours::transparentBlack);
        pole.slider.setColour (juce::Slider::thumbColourId, pole.colour);
        pole.slider.onValueChange = [this] { repaint(); };
        addAndMakeVisible (pole.slider);
    }
}

VelocityGraphPanel::~VelocityGraphPanel() = default;

void VelocityGraphPanel::layoutGraphArea()
{
    auto bounds = getLocalBounds();
    bounds.removeFromTop (30); // header row (title + zoom controls)
    bounds.reduce (44, 14);    // leave room for axis labels left of the plot
    graphArea = bounds;
}

void VelocityGraphPanel::resized()
{
    auto bounds = getLocalBounds();
    auto headerRow = bounds.removeFromTop (30).reduced (14, 4);

    zoomOutButton.setBounds (headerRow.removeFromRight (18));
    headerRow.removeFromRight (4);
    zoomInButton.setBounds (headerRow.removeFromRight (18));

    canvasTitle.setBounds (headerRow.removeFromLeft (70));
    dropdownArrow.setBounds (headerRow.removeFromLeft (16));

    layoutGraphArea();

    // Distribute the three pole sliders evenly across the graph width.
    const int n = (int) poles.size();
    const int slotWidth = graphArea.getWidth() / (n + 1);
    for (int i = 0; i < n; ++i)
    {
        const int cx = graphArea.getX() + slotWidth * (i + 1);
        auto sliderBounds = juce::Rectangle<int> (cx - 12, graphArea.getY(), 24, graphArea.getHeight());
        poles[(size_t) i].slider.setBounds (sliderBounds);
    }
}

void VelocityGraphPanel::paint (juce::Graphics& g)
{
    g.setColour (Colour::panelGreen);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), Layout::cornerRadiusPanel);

    // Dropdown chevron next to "Velocity" title
    {
        auto b = dropdownArrow.getBounds().toFloat();
        juce::Path p;
        auto c = b.getCentre();
        p.startNewSubPath (c.x - 4.0f, c.y - 2.0f);
        p.lineTo (c.x, c.y + 2.5f);
        p.lineTo (c.x + 4.0f, c.y - 2.0f);
        g.setColour (Colour::textDarkGreen);
        g.strokePath (p, juce::PathStrokeType (1.4f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }

    // Zoom +/- pill buttons (round outline with glyph)
    auto drawZoomGlyph = [&g] (juce::Rectangle<int> b, bool plus)
    {
        auto fb = b.toFloat().reduced (1.0f);
        g.setColour (Colour::pillBackground);
        g.drawEllipse (fb, 1.2f);
        g.setColour (Colour::textForestGreen);
        auto c = fb.getCentre();
        const float s = fb.getWidth() * 0.24f;
        g.drawLine (c.x - s, c.y, c.x + s, c.y, 1.4f);
        if (plus)
            g.drawLine (c.x, c.y - s, c.x, c.y + s, 1.4f);
    };
    drawZoomGlyph (zoomInButton.getBounds(), true);
    drawZoomGlyph (zoomOutButton.getBounds(), false);

    // Horizontal gridlines (4 lines dividing the 0-127 range, matching the
    // 4 faint rows visible in the reference screenshot)
    g.setColour (Colour::gridLine);
    const int numLines = 4;
    for (int i = 0; i <= numLines; ++i)
    {
        const float y = graphArea.getY() + graphArea.getHeight() * ((float) i / (float) numLines);
        g.drawLine ((float) graphArea.getX(), y, (float) graphArea.getRight(), y, 0.6f);
    }

    // Axis labels: 127 top, 65 middle, 0 bottom (matches reference art)
    g.setColour (Colour::textForestGreen);
    g.setFont (getUIFont (10.0f, true));
    auto labelColumn = juce::Rectangle<int> (graphArea.getRight() + 6, graphArea.getY() - 6,
                                              32, graphArea.getHeight() + 12);
    g.drawText ("127", labelColumn.withHeight (16), juce::Justification::topLeft);
    g.drawText ("65",  labelColumn.withY (labelColumn.getY() + graphArea.getHeight() / 2 - 8).withHeight (16),
                juce::Justification::topLeft);
    g.drawText ("0",   labelColumn.withY (labelColumn.getBottom() - 16).withHeight (16),
                juce::Justification::topLeft);

    // Poles: stem line from the bottom of the graph up to the handle,
    // topped with a filled circular handle outlined in white.
    for (auto& pole : poles)
    {
        auto sb = pole.slider.getBounds();
        const float cx = (float) sb.getCentreX();
        const auto range = pole.slider.getRange();
        const float t = (float) ((pole.slider.getValue() - range.getStart()) / (range.getEnd() - range.getStart()));
        const float top = (float) graphArea.getY();
        const float bottom = (float) graphArea.getBottom();
        const float handleY = bottom - t * (bottom - top);

        g.setColour (pole.colour);
        g.drawLine (cx, handleY, cx, bottom, 2.2f);

        const float handleR = 7.0f;
        auto handleBounds = juce::Rectangle<float> (cx - handleR, handleY - handleR, handleR * 2.0f, handleR * 2.0f);
        g.setColour (pole.colour);
        g.fillEllipse (handleBounds);
        g.setColour (Colour::white);
        g.drawEllipse (handleBounds, 2.0f);
    }
}

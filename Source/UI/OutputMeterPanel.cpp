#include "OutputMeterPanel.h"
#include "../LookAndFeel/HuhLookAndFeel.h"
#include "../PluginProcessor.h"

using namespace HuhTokens;

OutputMeterPanel::OutputMeterPanel (juce::AudioProcessorValueTreeState& apvts) : state (apvts)
{
    outputKnob.getProperties().set (HuhLookAndFeel::stylePropertyId, HuhLookAndFeel::styleOutputKnob);
    outputKnob.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                     juce::MathConstants<float>::pi * 2.8f,
                                     true);
    addAndMakeVisible (outputKnob);

    outputLabel.setJustificationType (juce::Justification::centred);
    outputLabel.setColour (juce::Label::textColourId, Colour::textDarkGreen);
    outputLabel.setFont (getUIFont (10.5f, true));
    outputLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (outputLabel);

    outputAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        state, HuhDelayAudioProcessor::ParamIDs::outputGain, outputKnob);

    // Idle animated shimmer so the meter doesn't look dead with no audio
    // engine behind it yet; replace with real peak data once DSP lands.
    startTimerHz (30);
}

OutputMeterPanel::~OutputMeterPanel()
{
    stopTimer();
}

void OutputMeterPanel::setMeterLevels (float leftLevel0to1, float rightLevel0to1)
{
    meterLeft  = juce::jlimit (0.0f, 1.0f, leftLevel0to1);
    meterRight = juce::jlimit (0.0f, 1.0f, rightLevel0to1);
    repaint();
}

void OutputMeterPanel::timerCallback()
{
    // Placeholder idle motion (no audio engine yet) - gently pulses so the
    // meter reads as "alive" during UI review. Safe to delete once
    // setMeterLevels() is driven by real processBlock() data.
    static float phase = 0.0f;
    phase += 0.05f;
    const float l = 0.45f + 0.25f * std::sin (phase);
    const float r = 0.45f + 0.25f * std::sin (phase + 0.6f);
    setMeterLevels (l, r);
}

void OutputMeterPanel::resized()
{
    auto bounds = getLocalBounds().reduced (10, 12);

    auto meterArea = bounds.removeFromTop (bounds.getHeight() - 70);
    juce::ignoreUnused (meterArea); // meter bars are custom-painted directly from getLocalBounds() in paint()

    bounds.removeFromTop (10);
    auto knobArea = bounds.removeFromTop (54).withSizeKeepingCentre (54, 54);
    outputKnob.setBounds (knobArea);
    outputLabel.setBounds (bounds.removeFromTop (16));
}

void OutputMeterPanel::paint (juce::Graphics& g)
{
    g.setColour (Colour::panelGreen);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), Layout::cornerRadiusPanel);

    auto bounds = getLocalBounds().reduced (10, 12);
    auto meterArea = bounds.removeFromTop (bounds.getHeight() - 70);

    const float barWidth = 8.0f;
    const float gap = 6.0f;
    const float totalWidth = barWidth * 2.0f + gap;
    auto barsBounds = meterArea.toFloat().withSizeKeepingCentre (totalWidth, meterArea.getHeight() * 0.92f);

    auto paintBar = [&g, &barsBounds] (float x, float level)
    {
        const float barWidthLocal = 8.0f;
        juce::Rectangle<float> track (x, barsBounds.getY(), barWidthLocal, barsBounds.getHeight());

        juce::ColourGradient grad (Colour::meterGreen, track.getX(), track.getBottom(),
                                    Colour::meterRed, track.getX(), track.getY(), false);
        grad.addColour (0.5, Colour::meterYellow);

        // Background track (dim)
        g.setColour (Colour::outerFrame.withAlpha (0.25f));
        g.fillRoundedRectangle (track, 2.0f);

        // Filled portion from the bottom, clipped to `level`
        auto filled = track.withY (track.getBottom() - track.getHeight() * level)
                            .withHeight (track.getHeight() * level);
        {
            juce::Graphics::ScopedSaveState save (g);
            g.reduceClipRegion (filled.getSmallestIntegerContainer());
            g.setGradientFill (grad);
            g.fillRoundedRectangle (track, 2.0f);
        }
    };

    paintBar (barsBounds.getX(), meterLeft);
    paintBar (barsBounds.getX() + barWidth + gap, meterRight);
}

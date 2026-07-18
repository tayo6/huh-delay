#include "DelayControlPanel.h"
#include "../LookAndFeel/HuhLookAndFeel.h"
#include "../PluginProcessor.h"

using namespace HuhTokens;

DelayControlPanel::DelayControlPanel (juce::AudioProcessorValueTreeState& apvts) : state (apvts)
{
    titleLabel.setJustificationType (juce::Justification::centredLeft);
    titleLabel.setColour (juce::Label::textColourId, Colour::textMutedGreen);
    titleLabel.setFont (getUIFont (24.0f, true));
    addAndMakeVisible (titleLabel);

    versionLabel.setJustificationType (juce::Justification::centredLeft);
    versionLabel.setColour (juce::Label::textColourId, Colour::textMutedGreen);
    versionLabel.setFont (getUIFont (9.0f));
    addAndMakeVisible (versionLabel);

    delayTimeKnob.getProperties().set (HuhLookAndFeel::stylePropertyId, HuhLookAndFeel::styleDelayKnob);
    delayTimeKnob.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                                        juce::MathConstants<float>::pi * 2.8f,
                                        true);
    addAndMakeVisible (delayTimeKnob);

    delayValueLabel.setJustificationType (juce::Justification::centred);
    delayValueLabel.setColour (juce::Label::textColourId, Colour::textDarkGreen);
    delayValueLabel.setFont (getUIFont (12.0f, true));
    delayValueLabel.setInterceptsMouseClicks (false, false);
    addAndMakeVisible (delayValueLabel);

    syncButton.getProperties().set (HuhLookAndFeel::stylePropertyId, HuhLookAndFeel::styleSyncPill);
    syncButton.setButtonText ("Sync");
    syncButton.setColour (juce::ToggleButton::textColourId, Colour::white);
    addAndMakeVisible (syncButton);

    delayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        state, HuhDelayAudioProcessor::ParamIDs::delayTime, delayTimeKnob);
    syncAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment> (
        state, HuhDelayAudioProcessor::ParamIDs::syncEnabled, syncButton);

    delayTimeKnob.addListener (this);
    updateDelayValueLabel();
}

DelayControlPanel::~DelayControlPanel()
{
    delayTimeKnob.removeListener (this);
}

void DelayControlPanel::updateDelayValueLabel()
{
    if (auto* param = state.getParameter (HuhDelayAudioProcessor::ParamIDs::delayTime))
        delayValueLabel.setText (param->getCurrentValueAsText(), juce::dontSendNotification);
}

void DelayControlPanel::resized()
{
    auto bounds = getLocalBounds().reduced (18, 14);

    auto titleRow = bounds.removeFromTop (30);
    titleLabel.setBounds (titleRow.removeFromLeft (95));
    versionLabel.setBounds (titleRow.removeFromLeft (40).translated (-4, 10));

    bounds.removeFromTop (6);

    const int knobSize = juce::jmin (bounds.getWidth() - 20, 110);
    auto knobArea = bounds.removeFromTop (knobSize).withSizeKeepingCentre (knobSize, knobSize);
    delayTimeKnob.setBounds (knobArea);

    bounds.removeFromTop (6);
    delayValueLabel.setBounds (bounds.removeFromTop (20));

    bounds.removeFromTop (8);

    auto syncArea = bounds.removeFromTop (26).withSizeKeepingCentre (juce::jmin (bounds.getWidth(), 150), 26);
    syncButton.setBounds (syncArea);
}

void DelayControlPanel::paint (juce::Graphics& g)
{
    g.setColour (Colour::panelGreen);
    g.fillRoundedRectangle (getLocalBounds().toFloat(), Layout::cornerRadiusPanel);
}

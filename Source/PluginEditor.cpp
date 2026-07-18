#include "PluginEditor.h"

using namespace HuhTokens;

HuhDelayAudioProcessorEditor::HuhDelayAudioProcessorEditor (HuhDelayAudioProcessor& p)
    : AudioProcessorEditor (&p),
      processor (p),
      headerBar (p.apvts),
      delayPanel (p.apvts),
      meterPanel (p.apvts)
{
    setLookAndFeel (&lookAndFeel);

    addAndMakeVisible (headerBar);
    addAndMakeVisible (delayPanel);
    addAndMakeVisible (velocityPanel);
    addAndMakeVisible (meterPanel);

    headerBar.onPresetStep = [] (int /*direction*/)
    {
        // TODO: wire up to a real preset manager. Left as a stub since
        // preset browsing/storage is outside this task's UI-only scope.
    };
    headerBar.onUndo = [] { /* TODO: hook into APVTS undo manager */ };
    headerBar.onRedo = [] { /* TODO: hook into APVTS undo manager */ };
    headerBar.onHelp = [] { juce::URL ("https://example.com/huh-delay-help").launchInDefaultBrowser(); };

    velocityPanel.onZoomIn  = [] { /* TODO: zoom the velocity canvas */ };
    velocityPanel.onZoomOut = [] { /* TODO: zoom the velocity canvas */ };

    setResizable (false, false);
    setSize (Layout::windowWidth, Layout::windowHeight);
}

HuhDelayAudioProcessorEditor::~HuhDelayAudioProcessorEditor()
{
    setLookAndFeel (nullptr);
}

void HuhDelayAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Outer rounded-rect frame (the dark olive border visible around the
    // whole plugin window in the reference art).
    g.fillAll (Colour::outerFrame);

    auto outer = getLocalBounds().toFloat().reduced (1.0f);
    g.setColour (Colour::outerFrame);
    g.fillRoundedRectangle (outer, Layout::cornerRadiusOuter);
}

void HuhDelayAudioProcessorEditor::resized()
{
    // Header strip runs full-width across the top.
    headerBar.setBounds (0, 0, getWidth(), Layout::headerHeight);

    // The three body panels are positioned from the fixed pixel constants
    // measured directly against the 1032x349 reference PNG, rather than
    // derived by subdividing remaining space - this keeps the small dark
    // gaps between panels pixel-accurate even if the window were ever
    // made resizable in the future.
    delayPanel.setBounds (Layout::leftPanelX, Layout::bodyY,
                           Layout::leftPanelWidth, Layout::bodyHeight);

    velocityPanel.setBounds (Layout::mainPanelX, Layout::bodyY,
                              Layout::mainPanelWidth, Layout::bodyHeight);

    meterPanel.setBounds (Layout::rightPanelX, Layout::bodyY,
                           Layout::rightPanelWidth, Layout::bodyHeight);
}

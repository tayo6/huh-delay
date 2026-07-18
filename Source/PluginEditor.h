#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "LookAndFeel/HuhLookAndFeel.h"
#include "LookAndFeel/DesignTokens.h"
#include "UI/HeaderBar.h"
#include "UI/DelayControlPanel.h"
#include "UI/VelocityGraphPanel.h"
#include "UI/OutputMeterPanel.h"

/**
    HuhDelayAudioProcessorEditor
    ------------------------------
    Fixed-size (1032 x 349 px) editor window reproducing the reference
    delay_ui.svg / delay_ui.png design pixel-for-pixel:

      +------------------------------------------------------------+
      | [<][>]     Frizzy [Drum]      (undo)(redo) (A)(B) (?) (pwr) |  <- HeaderBar
      +--------------+-----------------------------------+----------+
      |  HUH  v0.1   |  Velocity  [+][-]                  | | | (L/R
      |              |  127 ------------------------------|| |  meter)
      |   ( knob )   |   65 ------------------------------||
      |   1 / 16 D   |    0 ------------------------------||
      |  ( Sync )    |                                     |  (Output)
      +--------------+-------------------------------------+----------+

    All layout/colour constants live in LookAndFeel/DesignTokens.h so this
    class stays focused on composition and sizing rather than pixel
    values.
*/
class HuhDelayAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit HuhDelayAudioProcessorEditor (HuhDelayAudioProcessor&);
    ~HuhDelayAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    HuhDelayAudioProcessor& processor;

    HuhLookAndFeel lookAndFeel;

    HeaderBar           headerBar;
    DelayControlPanel    delayPanel;
    VelocityGraphPanel   velocityPanel;
    OutputMeterPanel      meterPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HuhDelayAudioProcessorEditor)
};

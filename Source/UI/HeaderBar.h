#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "../LookAndFeel/DesignTokens.h"

/**
    HeaderBar
    ---------
    The dark strip running along the very top of the plugin window,
    containing (left to right):
      [ < ]  [ > ]   Frizzy [Drum]        (undo) (redo)   (A)(B)   (?)  (power)

    This component only handles the "chrome" controls that live in the
    header strip itself (prev/next preset step arrows, undo, redo, A/B
    comparison toggle, help, power/bypass). The preset name + category
    pill living in the header is delegated to PresetBar, composed here,
    so each piece stays independently testable/reusable.
*/
class HeaderBar final : public juce::Component
{
public:
    explicit HeaderBar (juce::AudioProcessorValueTreeState& apvts);
    ~HeaderBar() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    // Fired when the user clicks the previous/next preset chevrons.
    std::function<void (int direction)> onPresetStep; // direction = -1 or +1
    std::function<void()> onUndo;
    std::function<void()> onRedo;
    std::function<void()> onHelp;

private:
    // All button text below is cleared at construction time; every
    // visible pixel for these controls is hand-painted in
    // HeaderBar::paint() (see makeInvisibleClickTarget in the .cpp).
    juce::TextButton prevButton;
    juce::TextButton nextButton;

    std::unique_ptr<class PresetBar> presetBar;

    // Text is left empty; paint() draws the curved undo/redo arrow
    // glyphs by hand so no font glyph-coverage assumptions are needed.
    juce::TextButton undoButton;
    juce::TextButton redoButton;

    juce::TextButton buttonA;
    juce::TextButton buttonB;

    juce::TextButton helpButton;
    juce::ToggleButton powerButton;

    juce::AudioProcessorValueTreeState& state;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> powerAttachment;

    void drawChevron (juce::Graphics&, juce::Rectangle<float> bounds, bool pointingRight);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (HeaderBar)
};

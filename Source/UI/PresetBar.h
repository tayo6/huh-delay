#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "../LookAndFeel/DesignTokens.h"

/**
    PresetBar
    ---------
    The pale pill in the centre of the header strip showing the current
    preset name ("Frizzy") and its category badge ("Drum"), plus the small
    left/right chevrons used to step through the preset list.

    This is a self-contained, presentation-only widget: it exposes the
    preset name/category as settable strings and reports clicks via
    callbacks, so PluginEditor / HeaderBar can wire it up to whatever
    preset-management backend gets added later.
*/
class PresetBar final : public juce::Component
{
public:
    PresetBar();
    ~PresetBar() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    void setPresetName (const juce::String& name);
    void setCategoryName (const juce::String& category);

    std::function<void (int direction)> onStep; // -1 previous, +1 next

private:
    juce::String presetName  { "Frizzy" };
    juce::String categoryName { "Drum" };

    juce::TextButton leftChevron  { "<" };
    juce::TextButton rightChevron { ">" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PresetBar)
};

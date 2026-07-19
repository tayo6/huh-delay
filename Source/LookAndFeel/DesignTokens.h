#pragma once

#include <juce_graphics/juce_graphics.h>

/**
    DesignTokens
    ------------
    Every colour, font size, and layout constant lifted directly from the
    supplied delay_ui.svg / delay_ui.png reference. Centralising these means
    every UI/ component and the LookAndFeel draw from one source of truth
    instead of scattering magic hex codes across files.

    Plugin window size is fixed at 1032 x 349 px, matching the reference
    PNG pixel-for-pixel (SVG viewBox 273.05mm x 92.372mm @ 96dpi == 1032x349).
*/
namespace HuhTokens
{
    //==============================================================================
    // Colours (hex values taken verbatim from delay_ui.svg fill/stroke attrs)
    namespace Colour
    {
        const juce::Colour outerFrame        (0xff3e4837); // outer rounded rect + dark bg
        const juce::Colour panelGreen         (0xff9dac93); // left / main / right panel fill
        const juce::Colour panelGreenDark     (0xff7c916f); // top header strip fill
        const juce::Colour textDarkGreen      (0xff3e4837); // primary readout text
        const juce::Colour textMutedGreen     (0xff5d6c53); // "HUH" title / v0.1 text
        const juce::Colour textForestGreen    (0xff37483e); // header icon glyphs, delay text
        const juce::Colour presetGreenAccent  (0xff217844); // "Drum" category pill text/stroke
        const juce::Colour gridLine           (0xffdbe3de); // velocity graph horizontal gridlines
        const juce::Colour pillBackground     (0xffb7c8be); // undo/redo/AB/zoom pill backgrounds
        const juce::Colour pillBackgroundAlt  (0xffbec8b7); // preset selector pill background
        const juce::Colour knobFace           (0xfff1f1f1); // delay knob face (gradient end)
        const juce::Colour knobFaceShadow     (0xffc0c0c0); // delay knob face (gradient start)
        const juce::Colour syncPillFill       (0xff5d6c53); // Sync toggle pill background
        const juce::Colour syncPillRing       (0xffececec); // Sync toggle inner ring (off state)
        const juce::Colour white              (0xffffffff);
        const juce::Colour cyanStub           (0xff00ffff); // velocity pole stem base colour
        const juce::Colour orangePole         (0xffff7f2a);
        const juce::Colour redPole            (0xffff2a2a);
        const juce::Colour greenPole          (0xff2ca02c);
        const juce::Colour meterGreen         (0xff00ff00);
        const juce::Colour meterYellow        (0xffffff00);
        const juce::Colour meterRed           (0xffff0000);
        const juce::Colour outputRing         (0xffff0000);
    }

    //==============================================================================
    // Fonts. "Avenir Next" is used throughout the SVG; we fall back through a
    // cross-platform-safe stack so CI builds (Linux/Windows/macOS runners
    // without Avenir Next installed) still render something close.
    inline juce::Font getUIFont (float heightPx, bool bold = false)
    {
        // NOTE: targeting JUCE 7.0.12 - this predates the juce::FontOptions
        // API (introduced in JUCE 8), so we use the classic
        // Font (typefaceName, height, styleFlags) constructor instead.
        static const juce::String preferredTypeface = []() -> juce::String
        {
            const juce::StringArray preferredFaces { "Avenir Next", "Helvetica Neue", "Arial", "Segoe UI" };
            const auto available = juce::Font::findAllTypefaceNames();
            for (auto& name : preferredFaces)
                if (available.contains (name))
                    return name;
            return juce::Font::getDefaultSansSerifFontName();
        }();

        const int styleFlags = bold ? juce::Font::bold : juce::Font::plain;
        juce::Font f (preferredTypeface, heightPx, styleFlags);
        f.setBold (bold);
        return f;
    }

    //==============================================================================
    // Layout constants (px, measured directly against the 1032x349 reference)
    namespace Layout
    {
        // Window
        constexpr int windowWidth  = 1032;
        constexpr int windowHeight = 349;

        constexpr float cornerRadiusOuter = 8.0f;
        constexpr float cornerRadiusPanel = 7.5f;

        // Header strip
        constexpr int headerHeight = 43;

        // Three-panel body, all starting at y = headerHeight
        constexpr int bodyY      = 43;
        constexpr int bodyHeight = windowHeight - bodyY - 6; // ~300

        constexpr int leftPanelX     = 6;
        constexpr int leftPanelWidth = 216;   // spans px 6 -> 222

        constexpr int mainPanelX     = 228;
        constexpr int mainPanelWidth = 720;   // spans px 228 -> 948

        constexpr int rightPanelX     = 954;
        constexpr int rightPanelWidth = 73;   // spans px 954 -> 1027
    }
}

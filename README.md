# HUH Delay (v0.1) — VST3 UI shell

A JUCE 7.0.12 VST3 mono delay plugin. **This pass implements the UI only** —
every knob, button, and slider you see is wired to a real, host-automatable
parameter via `AudioProcessorValueTreeState`, so the plugin is fully
interactive in a host / plugin validator, but `processBlock()` is a
passthrough stub. The actual delay DSP is a follow-up task (see the `TODO`
comments in `Source/PluginProcessor.cpp`/`.h`).

The interface is a pixel-accurate reproduction of the supplied
`delay_ui.svg` / `delay_ui.png` reference: a fixed **1032 × 349 px** window
with a dark header strip, a left "HUH" delay-time panel, a central velocity
graph panel, and a right-hand output meter + gain panel.

## Project layout

```
CMakeLists.txt                 Top-level build script (JUCE + plugin target)
Source/
  PluginProcessor.h/.cpp       AudioProcessor: parameter layout, passthrough audio
  PluginEditor.h/.cpp          Top-level editor: composes the four UI panels
  LookAndFeel/
    DesignTokens.h             Every colour / font / layout constant, one source of truth
    HuhLookAndFeel.h/.cpp      Custom LookAndFeel_V4: rotary knobs, toggle pills, power button
  UI/
    HeaderBar.h/.cpp           Top strip: prev/next, undo/redo, A/B, help, power
    PresetBar.h/.cpp           "Frizzy [Drum]" preset name/category pill
    DelayControlPanel.h/.cpp   Left panel: HUH title, delay-time knob, Sync toggle
    VelocityGraphPanel.h/.cpp  Centre panel: velocity graph, gridlines, 3 poles, zoom
    OutputMeterPanel.h/.cpp    Right panel: L/R level meter, Output gain knob
.github/workflows/build.yml    CI: builds VST3 on Linux / macOS / Windows
```

Each `UI/` component is a self-contained `juce::Component` with its own
header/cpp, taking only the `AudioProcessorValueTreeState` reference it
actually needs to bind its own controls — nothing reaches into a sibling
component's internals, so panels can be reordered, resized, or reused
independently.

## Building locally

```bash
git clone <this-repo> huh-delay
cd huh-delay
git clone --branch 7.0.12 --depth 1 https://github.com/juce-framework/JUCE.git JUCE

cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

The built `.vst3` bundle will be under `build/HUH_Delay_artefacts/`.

## Building in CI

`.github/workflows/build.yml` checks out this repo and a pinned
`7.0.12` tag of JUCE side-by-side, then runs the same CMake configure/build
commands above on Linux, macOS, and Windows runners, uploading the `.vst3`
bundle from each platform as a workflow artifact.

## Parameters

| Parameter ID  | Type   | Range / choices                        | Default   | UI control            |
|---------------|--------|-----------------------------------------|-----------|------------------------|
| `delayTime`   | Choice | 1/32 … 1/1 (12 musical divisions)       | `1 / 16 D`| Left-panel rotary knob |
| `syncEnabled` | Bool   | on / off                                | `true`    | "Sync" pill toggle     |
| `outputGain`  | Float  | -24 dB … +6 dB                          | `0 dB`    | Output knob (right)    |
| `bypass`      | Bool   | on / off                                | `false`   | Power button (header)  |

## What's stubbed / not implemented yet

- **Audio DSP**: `processBlock()` passes audio through unchanged. No delay
  line, feedback, or filtering exists yet.
- **Preset browsing**: the `<`/`>` chevrons and "Frizzy [Drum]" pill are
  visual + click-callback-ready, but there's no preset storage/list behind
  them.
- **Undo/redo**: buttons exist and fire callbacks; hook these into
  `AudioProcessorValueTreeState::getUndoManager()` when ready.
- **A/B comparison**: the header toggle switches visually; snapshotting/
  restoring full plugin state per slot is not implemented.
- **Velocity graph poles**: fully interactive (drag to change value 0-127)
  but not yet connected to any MIDI velocity data model — currently a local
  per-pole value used only for the visual demo.
- **Output meter**: currently driven by a gentle idle sine animation rather
  than real peak/RMS data, so the UI doesn't look dead with no DSP behind
  it. Swap the `Timer` callback in `OutputMeterPanel` for real levels once
  `processBlock()` does something.

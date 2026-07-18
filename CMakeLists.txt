cmake_minimum_required(VERSION 3.22)

project(HUH_Delay VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# JUCE is pulled in as a submodule at ./JUCE (pinned to 7.0.12 by the CI
# workflow / by the developer when cloning locally). See README.md for the
# exact commands used to fetch it if you are building outside of CI.
# ---------------------------------------------------------------------------
add_subdirectory(JUCE)

juce_add_plugin(HUH_Delay
    COMPANY_NAME                  "YourCompany"
    PLUGIN_MANUFACTURER_CODE      Ycmp
    PLUGIN_CODE                   Hdl1
    FORMATS                       VST3
    PRODUCT_NAME                  "HUH"
    IS_SYNTH                      FALSE
    NEEDS_MIDI_INPUT              FALSE
    NEEDS_MIDI_OUTPUT             FALSE
    IS_MIDI_EFFECT                FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS   FALSE
    COPY_PLUGIN_AFTER_BUILD       FALSE
    VST3_CATEGORIES                Delay Fx
)

# Give the plugin a modern JUCE header + JUCE version define, used by
# PluginEditor.cpp for the "v0.1" style version label building block.
juce_generate_juce_header(HUH_Delay)

target_sources(HUH_Delay
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
        Source/LookAndFeel/HuhLookAndFeel.cpp
        Source/UI/HeaderBar.cpp
        Source/UI/PresetBar.cpp
        Source/UI/DelayControlPanel.cpp
        Source/UI/VelocityGraphPanel.cpp
        Source/UI/OutputMeterPanel.cpp
)

target_compile_definitions(HUH_Delay
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_include_directories(HUH_Delay
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

target_link_libraries(HUH_Delay
    PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

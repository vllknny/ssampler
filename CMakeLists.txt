cmake_minimum_required(VERSION 3.15)

project(StemSplitterSampler VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(JUCE)

juce_add_plugin(StemSplitterSampler
    COMPANY_NAME "Audio Tools"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_COPY_DIR "~/Library/Audio/Plug-Ins/VST3" 
    AU_COPY_DIR "~/Library/Audio/Plug-Ins/Components"
    FORMATS VST3
    PLUGIN_MANUFACTURER_CODE AudT
    PLUGIN_CODE SSSS
    IS_AU_MANUFACTURER TRUE)

target_sources(StemSplitterSampler
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessor.h
        Source/PluginEditor.cpp
        Source/PluginEditor.h
        Source/StemSeparator.cpp
        Source/StemSeparator.h
        Source/SamplerComponent.cpp
        Source/SamplerComponent.h
        Source/DemucsInterface.cpp
        Source/DemucsInterface.h)

target_link_libraries(StemSplitterSampler
    PRIVATE
        PUBLIC juce::juce_audio_utils
        PUBLIC juce::juce_dsp
        PUBLIC juce::juce_recommended_config_flags
        PUBLIC juce::juce_recommended_lto_flags
        PUBLIC juce::juce_recommended_warning_flags)

target_compile_definitions(StemSplitterSampler
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0)
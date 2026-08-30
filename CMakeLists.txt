cmake_minimum_required(VERSION 3.20)

project(MAZYES_VOCALS VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to use local JUCE directory if provided, or FetchContent from GitHub
option(JUCE_USE_FETCHCONTENT "Fetch JUCE from GitHub if not found locally" ON)

if (TARGET juce::juce_core)
    message(STATUS "JUCE already available as target")
else()
    find_package(JUCE CONFIG QUIET)
    if (NOT JUCE_FOUND AND JUCE_USE_FETCHCONTENT)
        include(FetchContent)
        message(STATUS "Fetching JUCE 7.0.12 from GitHub...")
        FetchContent_Declare(
            JUCE
            GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
            GIT_TAG 7.0.12
            GIT_SHALLOW TRUE
        )
        FetchContent_MakeAvailable(JUCE)
    endif()
endif()

# Define JUCE Plugin
juce_add_plugin(MazyesVocals
    COMPANY_NAME "Mazyes Audio"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Mazy
    PLUGIN_CODE MzVc
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "MAZYES VOCALS"
    DESCRIPTION "High-End Real-Time Vocal Processing Suite"
)

# Source Files
target_sources(MazyesVocals PRIVATE
    Source/PluginProcessor.h
    Source/PluginProcessor.cpp
    Source/PluginEditor.h
    Source/PluginEditor.cpp
    Source/Parameters.h
    Source/PresetManager.h
    
    # DSP
    Source/DSP/AudioBufferFIFO.h
    Source/DSP/PitchDetector.h
    Source/DSP/PitchShifter.h
    Source/DSP/PitchCorrectionEngine.h
    Source/DSP/NoiseGate.h
    Source/DSP/VocalEQ.h
    Source/DSP/VocalCompressor.h
    Source/DSP/VocalDeEsser.h
    Source/DSP/OversamplingWrapper.h
    Source/DSP/VocalSaturation.h
    Source/DSP/VocalExciter.h
    Source/DSP/VocalDelay.h
    Source/DSP/VocalReverb.h
    Source/DSP/VocalLimiter.h
    
    # UI
    Source/UI/MazyesLookAndFeel.h
    Source/UI/MazyesLookAndFeel.cpp
    Source/UI/RotaryKnob.h
    Source/UI/MeterComponent.h
    Source/UI/ModuleHeader.h
    Source/UI/PitchVisualizer.h
    Source/UI/PitchVisualizer.cpp
)

# Include Directories
target_include_directories(MazyesVocals PRIVATE
    Source
    Source/DSP
    Source/UI
)

# Compiler definitions
target_compile_definitions(MazyesVocals PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

# Link JUCE Modules
target_link_libraries(MazyesVocals PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

cmake_minimum_required(VERSION 3.15)

# ChopShop - Stutter/Glitch Effect Plugin
juce_add_plugin(ChopShop
    COMPANY_NAME "GHAAD"
    PLUGIN_MANUFACTURER_CODE Ghad
    PLUGIN_CODE Chop
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "ChopShop"
    NEEDS_MIDI_INPUT TRUE           # Required for MIDI trigger mode
    IS_SYNTH FALSE                  # Effect, not instrument
)

# Source files
target_sources(ChopShop
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Include paths
target_include_directories(ChopShop
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(ChopShop
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp                  # For DryWetMixer
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# Generate JuceHeader.h (JUCE 8 requirement - CRITICAL)
juce_generate_juce_header(ChopShop)

# Compile definitions
target_compile_definitions(ChopShop
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
)

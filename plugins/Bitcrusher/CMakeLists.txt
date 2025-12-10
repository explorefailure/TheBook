cmake_minimum_required(VERSION 3.15)

# Plugin formats: VST3, AU, Standalone
juce_add_plugin(Bitcrusher
    COMPANY_NAME "GHAAD"
    PLUGIN_MANUFACTURER_CODE Grot
    PLUGIN_CODE Btcr
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Bitcrusher"
)

# Source files
target_sources(Bitcrusher
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

# Include paths
target_include_directories(Bitcrusher
    PRIVATE
        Source
)

# Required JUCE modules
target_link_libraries(Bitcrusher
    PRIVATE
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_data_structures
        juce::juce_dsp
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# CRITICAL: Generate JuceHeader.h (JUCE 8 requirement)
juce_generate_juce_header(Bitcrusher)

# Compile definitions
target_compile_definitions(Bitcrusher
    PUBLIC
        JUCE_VST3_CAN_REPLACE_VST2=0
)

# WebView support (uncommented by gui-agent if WebView UI is used)
# target_compile_definitions(Bitcrusher
#     PUBLIC
#         JUCE_WEB_BROWSER=1
#         JUCE_USE_CURL=0
# )

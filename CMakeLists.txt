cmake_minimum_required(VERSION 3.22)
project(Orbish VERSION 0.6.2)

add_subdirectory(JUCE)

juce_add_module(${CMAKE_CURRENT_SOURCE_DIR}/ff_meters)

juce_add_plugin(Orbish
    COMPANY_NAME "Exu"
    BUNDLE_ID "exu.Orbish"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Exuo
    PLUGIN_CODE Orbi
    FORMATS Standalone VST3 AU
    PRODUCT_NAME "Orbish"
    PLUGIN_NAME "Orbish"
    PLUGIN_DESCRIPTION "Audio Looper Plugin"
    AU_MAIN_TYPE kAudioUnitType_Effect
    VST3_CATEGORIES "Fx"
    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(Orbish)

target_compile_features(Orbish PUBLIC cxx_std_17)

target_sources(Orbish
    PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginProcessorLifecycle.cpp
        Source/PluginProcessorCommands.cpp
        Source/PluginProcessorEvents.cpp
        Source/PluginProcessorMidiMapping.cpp
        Source/PluginEditor.cpp
        Source/Track.cpp
        Source/TrackGroup.cpp
        Source/TrackLoader.cpp
        Source/Loop.cpp
        Source/Layer.cpp
        Source/GainModifier.cpp
        Source/DataExchange.cpp
        Source/AudioImporter.cpp
        Source/MofflRAudioFormatReader.cpp
        Source/OrbishLogger.cpp
        Source/ParameterDefinitions.cpp
        Source/AllocatorThread.cpp
        Source/MidiMapping.cpp
        Source/MidiProcessor.cpp
        Source/InternalSynchronizer.cpp
        Source/HostSynchronizer.cpp
        Source/Realignment.cpp
        Source/ImageViewport.cpp
        Source/OrbishLevelMeter.cpp
        Source/OrbishLookAndFeel.cpp
        Source/OrbishSlider.cpp
        Source/CustomButton.cpp
        Source/MainMenu.cpp
        Source/SettingsPage.cpp
        Source/HeaderArea.cpp
        Source/InfoAndControlArea.cpp
        Source/InfoArea.cpp
        Source/ControlArea.cpp
        Source/ButtonControlArea.cpp
        Source/TransportControlArea.cpp
        Source/GlobalControlArea.cpp
        Source/ModeAndNavigationControlArea.cpp
        Source/ModeControlArea.cpp
        Source/NavigationControlArea.cpp
        Source/InputControlArea.cpp
        Source/OutputControlArea.cpp
        Source/ThumbnailAndGroupArea.cpp
        Source/ThumbnailArea.cpp
        Source/GroupControlArea.cpp
        Source/TrackArea.cpp
        Source/TrackComponent.cpp
        Source/LoopComponent.cpp
        Source/OpenGLComponent.cpp
        Source/OpenGLComponentContainer.cpp
        Source/OpenGLAudioThumbnail.cpp
        Source/OpenGLAudioMeter.cpp
        Source/OpenGLClickWitness.cpp
        Source/Shader.cpp
)

file(GLOB RESOURCE_IMAGES
    "resources/Bases/*.png"
    "resources/Bases/button-icons/*.png"
    "resources/Bases/layout-change/*.png"
    "resources/*.png"
)

file(GLOB RESOURCE_SHADERS
    "resources/shaders/*.fs"
    "resources/shaders/*.vs"
)

file(GLOB RESOURCE_AUDIO
    "resources/Bases/*.aif"
)

juce_add_binary_data(OrishBinaryData
    SOURCES
        ${RESOURCE_IMAGES}
        ${RESOURCE_SHADERS}
        ${RESOURCE_AUDIO}
)

target_link_libraries(Orbish
    PRIVATE
        OrishBinaryData
        ff_meters
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_plugin_client
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_core
        juce::juce_cryptography
        juce::juce_data_structures
        juce::juce_events
        juce::juce_graphics
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_opengl
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(Orbish
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_STRICT_REFCOUNTEDPOINTER=1
        JUCE_DISPLAY_SPLASH_SCREEN=1
        JUCE_APP_VERSION="${PROJECT_VERSION}"
        JUCE_APP_VERSION_HEX=0x602
)

target_include_directories(Orbish
    PRIVATE
        Source
        exu
        ${CMAKE_CURRENT_SOURCE_DIR}
)

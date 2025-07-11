cmake_minimum_required(VERSION 3.15)

project(Audiovisual_Plugin VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(extern/JUCE)

# Desativar VST2
set(JUCE_ENABLE_VST2 OFF)
set(JUCE_PLUGINHOST_VST OFF)




juce_add_plugin(Audiovisual_Plugin
        COMPANY_NAME "Plugin_AcidSoundWorks"
        IS_SYNTH FALSE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT TRUE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS FALSE
        COPY_PLUGIN_AFTER_BUILD TRUE
        PLUGIN_MANUFACTURER_CODE Juce
        PLUGIN_CODE MnPl
        FORMATS VST3 Standalone
        PRODUCT_NAME "Audiovisual Plugin"
)

target_sources(Audiovisual_Plugin PRIVATE
        Source/PluginProcessor.cpp
        Source/PluginEditor.cpp
)

target_link_libraries(Audiovisual_Plugin PRIVATE
        juce::juce_audio_utils
        juce::juce_audio_processors
        juce::juce_dsp
)

target_compile_definitions(Audiovisual_Plugin PRIVATE JUCE_VST3_CAN_REPLACE_VST2=0)

if (UNIX AND NOT APPLE)
    message(STATUS "GTK wird auf Linux hinzugefügt...")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
    pkg_check_modules(WEBKIT2 REQUIRED webkit2gtk-4.1)
    find_package(CURL REQUIRED)

    include_directories(${GTK3_INCLUDE_DIRS})
    include_directories(${WEBKIT2_INCLUDE_DIRS})

    link_directories(${GTK3_LIBRARY_DIRS})
    link_directories(${WEBKIT2_LIBRARY_DIRS})

    add_definitions(${GTK3_CFLAGS_OTHER})
    add_definitions(${WEBKIT2_CFLAGS_OTHER})

    target_link_libraries(Audiovisual_Plugin PRIVATE ${GTK3_LIBRARIES})
    target_link_libraries(Audiovisual_Plugin PRIVATE ${WEBKIT2_LIBRARIES})
    target_link_libraries(Audiovisual_Plugin PRIVATE CURL::libcurl)
endif()

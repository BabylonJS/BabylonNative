set(SOURCES
    "Include/Babylon/Plugins/NativeAudio.h"
    "Source/NativeAudio.cpp")

add_library(NativeAudio ${SOURCES})

target_include_directories(NativeAudio PUBLIC "Include")

target_link_to_dependencies(NativeAudio
    PUBLIC JsRuntime
    PRIVATE LabSound)
#LabSound causes a number of unused parameter warnings that we consider errors. Disable this for now, and consider fixing this in LabSound.
#warnings_as_errors(NativeAudio)

set_property(TARGET NativeAudio PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(NativeAudioInternal INTERFACE)
target_include_directories(NativeAudioInternal INTERFACE "Source")
target_link_to_dependencies(NativeAudioInternal
    INTERFACE NativeAudio
    INTERFACE LabSound)

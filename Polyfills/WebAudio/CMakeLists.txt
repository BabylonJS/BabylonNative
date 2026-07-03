set(SOURCES
    "Include/Babylon/Polyfills/WebAudio.h"
    "Source/WebAudio.cpp"
    "Source/AudioEngine.cpp"
    "Source/AudioEngine.h"
    "Source/AudioNode.cpp"
    "Source/AudioNode.h"
    "Source/AudioParamState.cpp"
    "Source/AudioParam.cpp"
    "Source/AudioParam.h"
    "Source/AudioBuffer.cpp"
    "Source/AudioBuffer.h"
    "Source/AudioBufferSourceNode.cpp"
    "Source/AudioBufferSourceNode.h"
    "Source/GainNode.cpp"
    "Source/GainNode.h"
    "Source/PannerNode.cpp"
    "Source/PannerNode.h"
    "Source/StereoPannerNode.cpp"
    "Source/StereoPannerNode.h"
    "Source/AnalyserNode.cpp"
    "Source/AnalyserNode.h"
    "Source/AudioListener.cpp"
    "Source/AudioListener.h"
    "Source/AudioDestinationNode.cpp"
    "Source/AudioDestinationNode.h"
    "Source/AudioContext.cpp"
    "Source/AudioContext.h"
    "Source/HTMLAudioElement.cpp"
    "Source/HTMLAudioElement.h"
    "Source/AudioPlatform.cpp"
    "Source/miniaudio_impl.cpp")

add_library(WebAudio ${SOURCES})

warnings_as_errors(WebAudio)

target_include_directories(WebAudio
    PUBLIC "Include"
    PRIVATE "Source")

target_link_libraries(WebAudio
    PUBLIC napi
    PRIVATE JsRuntimeInternal
    PRIVATE miniaudio
    PRIVATE arcana)

set_property(TARGET WebAudio PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

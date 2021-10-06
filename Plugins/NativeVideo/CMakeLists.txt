set(SOURCES 
    "Include/Babylon/Plugins/NativeVideo.h"
    "Source/NativeVideo.cpp"
    "Source/NativeVideo.h")

if(APPLE)
    set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/NativeVideoImpl.mm")
else()
    set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/NativeVideoImpl.cpp")
endif()

set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/NativeVideoImpl.h")

add_library(NativeVideo ${SOURCES})
warnings_as_errors(NativeVideo)

target_include_directories(NativeVideo INTERFACE "Include")
target_include_directories(NativeVideo PRIVATE "Include/Babylon/Plugins"
    PRIVATE "Source/${BABYLON_NATIVE_PLATFORM}")

if(ANDROID)
elseif(APPLE)
    set(EXTENSIONS "-framework CoreMedia -framework AVFoundation")
endif()

target_link_to_dependencies(NativeVideo
    PUBLIC JsRuntime
    PRIVATE GraphicsInternal
    PRIVATE ${EXTENSIONS})

set_property(TARGET NativeVideo PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

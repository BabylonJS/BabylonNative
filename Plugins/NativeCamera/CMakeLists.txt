set(SOURCES 
    "Include/Babylon/Plugins/NativeCamera.h"
    "Source/NativeCamera.cpp"
    "Source/NativeVideo.cpp"
    "Source/NativeVideo.h")

if(APPLE)
    set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/NativeCameraImpl.mm")
else()
    set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/NativeCameraImpl.cpp")
endif()

set(SOURCES ${SOURCES} "Source/${BABYLON_NATIVE_PLATFORM}/NativeCameraImpl.h")

add_library(NativeCamera ${SOURCES})
warnings_as_errors(NativeCamera)

target_include_directories(NativeCamera INTERFACE "Include")
target_include_directories(NativeCamera PRIVATE "Include/Babylon/Plugins"
    PRIVATE "Source/${BABYLON_NATIVE_PLATFORM}")

if(ANDROID)
    set(EXTENSIONS AndroidExtensions)
    if(ANDROID_NATIVE_API_LEVEL GREATER_EQUAL 24)
        set(EXTENSIONS ${EXTENSIONS} camera2ndk)
    endif()
elseif(APPLE)
    set(EXTENSIONS "-framework CoreMedia -framework AVFoundation")
endif()

target_link_to_dependencies(NativeCamera
    PUBLIC JsRuntime
    PRIVATE GraphicsInternal
    PRIVATE NativeEngineInternal
    PRIVATE napi_extensions
    PRIVATE ${EXTENSIONS})

set_property(TARGET NativeCamera PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

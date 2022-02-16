if(APPLE)
    set(PLATFORM Apple)
else()
    set(PLATFORM ${BABYLON_NATIVE_PLATFORM})
endif()

set(SOURCES
    "Include/Babylon/Plugins/NativeCamera.h"
    "Source/NativeCamera.cpp"
    "Source/NativeVideo.cpp"
    "Source/NativeVideo.h"
    "Source/${PLATFORM}/NativeCameraImpl.h"
    "Source/${PLATFORM}/NativeCameraImpl.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}")

add_library(NativeCamera ${SOURCES})
warnings_as_errors(NativeCamera)

target_include_directories(NativeCamera
    INTERFACE "Include"
    PRIVATE "Include/Babylon/Plugins"
    PRIVATE "Source/${PLATFORM}")

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
    PRIVATE napi_extensions
    PRIVATE ${EXTENSIONS})

set_property(TARGET NativeCamera PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

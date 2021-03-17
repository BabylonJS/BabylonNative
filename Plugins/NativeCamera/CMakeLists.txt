set(SOURCES 
    "Include/Babylon/Plugins/NativeCamera.h"
    "Source/NativeCamera.cpp"
    "Source/NativeCamera.h"
    "Source/NativeVideo.cpp"
    "Source/NativeVideo.h")

if(APPLE)
    set(SOURCES ${SOURCES} "Source/NativeCamera${BABYLON_NATIVE_PLATFORM}.mm")
else()
    set(SOURCES ${SOURCES} "Source/NativeCamera${BABYLON_NATIVE_PLATFORM}.cpp")
endif()

if (APPLE OR ANDROID OR (WIN32 AND NOT WINDOWS_STORE))
    add_library(NativeCamera ${SOURCES})
    warnings_as_errors(NativeCamera)

    target_include_directories(NativeCamera INTERFACE "Include")
    target_include_directories(NativeCamera PRIVATE "Include/Babylon/Plugins")

    if(ANDROID)
        set(EXTENSIONS AndroidExtensions
            camera2ndk
            mediandk)
    elseif(APPLE)
        set(EXTENSIONS "-framework CoreMedia -framework AVFoundation")
    endif()

    target_link_to_dependencies(NativeCamera
        PUBLIC JsRuntime
        PRIVATE GraphicsInternal
        PRIVATE ${EXTENSIONS})

    set_property(TARGET NativeCamera PROPERTY FOLDER Plugins)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
endif()
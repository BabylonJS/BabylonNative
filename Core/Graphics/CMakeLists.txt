set(SOURCES
    "Include/Babylon/Graphics.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/Graphics.cpp"
    "Source/GraphicsImpl.cpp"
    "Source/GraphicsImpl.h"
    "Source/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatformImpl.h")

if(APPLE)
    if(IOS)
        set(SOURCES ${SOURCES}
            "Source/${BABYLON_NATIVE_PLATFORM}/iOS/Graphics.mm")
    else()
        set(SOURCES ${SOURCES}
            "Source/${BABYLON_NATIVE_PLATFORM}/macOS/Graphics.mm")
    endif()
else()
    set(SOURCES ${SOURCES}
        "Source/${BABYLON_NATIVE_PLATFORM}/Graphics.cpp")
endif()

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
            PUBLIC "Include"
            PRIVATE "Source/${BABYLON_NATIVE_PLATFORM}")


if(WIN32 AND NOT WINDOWS_STORE)
    target_link_to_dependencies(Graphics
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_to_dependencies(Graphics
        PRIVATE android
        PRIVATE AndroidExtensions)
elseif(UNIX AND NOT APPLE)
    # link to linux libraries
endif()

target_link_to_dependencies(Graphics
    PUBLIC JsRuntime
    PRIVATE JsRuntimeInternal
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

target_compile_definitions(Graphics
    PRIVATE NOMINMAX)

set_property(TARGET Graphics PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(GraphicsInternal INTERFACE)
target_include_directories(GraphicsInternal
    INTERFACE "Source"
    INTERFACE "Source/${BABYLON_NATIVE_PLATFORM}")
target_link_to_dependencies(GraphicsInternal
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx)

target_compile_definitions(GraphicsInternal
    INTERFACE NOMINMAX)

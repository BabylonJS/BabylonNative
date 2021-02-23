set(SOURCES
    "Include/Shared/Babylon/Graphics.h"
    "Include/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatform.h"
    "Source/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatformImpl.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/Graphics.cpp"
    "Source/GraphicsImpl.cpp"
    "Source/GraphicsImpl.h"
    "Source/${BABYLON_NATIVE_PLATFORM}/Graphics.cpp")

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
    PUBLIC "Include/Shared"
    PUBLIC "Include/${BABYLON_NATIVE_PLATFORM}"
    PRIVATE "Source/${BABYLON_NATIVE_PLATFORM}")

if(WINDOWS_STORE)
    # link to UWP libararies
elseif(WIN32)
    target_link_to_dependencies(Graphics
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_to_dependencies(Graphics
        PRIVATE android
        PRIVATE AndroidExtensions)
elseif(APPLE)
    # link to apple libraries
elseif(UNIX)
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

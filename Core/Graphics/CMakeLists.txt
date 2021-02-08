set(SOURCES
    "Include/Babylon/Graphics.h"
    "Include/Babylon/${BABYLON_NATIVE_PLATFORM}/GraphicsPlatform.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/Graphics.cpp"
    "Source/GraphicsImpl.h"
    "Source/${BABYLON_NATIVE_PLATFORM}/Graphics.cpp")

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
    PUBLIC "Include"
    PUBLIC "Include/Babylon/${BABYLON_NATIVE_PLATFORM}")

if(WINDOWS_STORE)
elseif(WIN32)
    target_link_to_dependencies(Graphics
        PRIVATE "shlwapi.lib")
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
target_include_directories(GraphicsInternal INTERFACE "Source")
target_link_to_dependencies(GraphicsInternal
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx)

    target_compile_definitions(GraphicsInternal
    INTERFACE NOMINMAX)

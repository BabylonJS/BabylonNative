set(SOURCES
    "Include/Babylon/NativeGraphics.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/NativeGraphics.cpp"
    "Source/NativeGraphicsImpl.h")

add_library(NativeGraphics ${SOURCES})
warnings_as_errors(NativeGraphics)

target_include_directories(NativeGraphics
    PUBLIC "Include")

target_link_to_dependencies(NativeGraphics
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

target_compile_definitions(NativeGraphics
    PRIVATE NOMINMAX)

set_property(TARGET NativeGraphics PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(NativeGraphicsInternal INTERFACE)
target_include_directories(NativeGraphicsInternal INTERFACE "Source")
target_link_to_dependencies(NativeGraphicsInternal
    INTERFACE NativeGraphics
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx)

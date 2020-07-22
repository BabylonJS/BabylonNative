set(SOURCES
    "Include/Babylon/NativeGraphics.h"
    "Source/NativeGraphics.cpp")

add_library(NativeGraphics ${SOURCES})
warnings_as_errors(NativeGraphics)

target_include_directories(NativeGraphics
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

target_link_to_dependencies(NativeGraphics
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PUBLIC JsRuntime)

target_compile_definitions(NativeGraphics
    PRIVATE NOMINMAX)

set_property(TARGET NativeGraphics PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Image.cpp"
    "Source/Image.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    "Source/nanovg_babylon.cpp"
    "Source/nanovg_babylon.h"
    )

file(GLOB FONT_SOURCES ${BGFX_DIR}/examples/common/font/*.cpp)
file(GLOB NANOVG_SOURCES ${BGFX_DIR}/examples/common/nanovg/nanovg.cpp)
set(ATLAS_SOURCES ${BGFX_DIR}/examples/common/cube_atlas.cpp)

add_library(Canvas ${SOURCES} ${FONT_SOURCES} ${ATLAS_SOURCES} ${NANOVG_SOURCES})

target_include_directories(Canvas PUBLIC "Include"
    PRIVATE "${BGFX_DIR}/3rdparty"
    PRIVATE "${BGFX_DIR}/examples/common"
    PRIVATE "${BGFX_DIR}/examples/common/nanovg")

target_link_to_dependencies(Canvas PUBLIC napi
    PUBLIC JsRuntime
    INTERFACE Graphics
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE GraphicsInternal
    PRIVATE NativeEngine
    PRIVATE NativeEngineInternal)

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("3rd party Sources" ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FONT_SOURCES} ${NANOVG_SOURCES} ${ATLAS_SOURCES})
# disable warnings because of 3rd party issue with anonymous struct
disable_warnings(Canvas)
target_compile_definitions(Canvas PRIVATE _CRT_SECURE_NO_WARNINGS)
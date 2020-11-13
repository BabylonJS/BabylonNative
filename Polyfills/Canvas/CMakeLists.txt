set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    )

file(GLOB FONT_SOURCES ${BGFX_DIR}/examples/common/font/*.cpp)
file(GLOB NANOVG_SOURCES ${BGFX_DIR}/examples/common/nanovg/*.cpp)
set(ATLAS_SOURCES ${BGFX_DIR}/examples/common/cube_atlas.cpp)

add_library(Canvas ${SOURCES} ${FONT_SOURCES} ${ATLAS_SOURCES} ${NANOVG_SOURCES})
warnings_as_errors(Canvas)

target_include_directories(Canvas PUBLIC "Include"
    PRIVATE "${BGFX_DIR}/3rdparty"
    PRIVATE "${BGFX_DIR}/examples/common")

target_link_to_dependencies(Canvas PUBLIC napi
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE Graphics)

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
# disable warnings because of 3rd party 
disable_warnings(Canvas)
target_compile_definitions(Canvas PRIVATE _CRT_SECURE_NO_WARNINGS)
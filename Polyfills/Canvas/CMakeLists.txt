set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Colors.h"
    "Source/Image.cpp"
    "Source/Image.h"
    "Source/ImageData.cpp"
    "Source/ImageData.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    "Source/nanovg_babylon.cpp"
    "Source/nanovg_babylon.h"
    )

file(GLOB SHADERS Source/*.sc)

file(GLOB FONT_SOURCES ${BGFX_DIR}/examples/common/font/*.cpp)
file(GLOB NANOVG_SOURCES ${BGFX_DIR}/examples/common/nanovg/nanovg.cpp)
set(ATLAS_SOURCES ${BGFX_DIR}/examples/common/cube_atlas.cpp)

add_library(Canvas ${SOURCES} ${FONT_SOURCES} ${ATLAS_SOURCES} ${NANOVG_SOURCES} ${SHADERS})

target_include_directories(Canvas
    PUBLIC "Include"
    PRIVATE "Source"
    PRIVATE "${BGFX_DIR}/3rdparty"
    PRIVATE "${BGFX_DIR}/examples/common"
    PRIVATE "${BGFX_DIR}/examples/common/nanovg")

target_link_libraries(Canvas
    PUBLIC napi
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext
    PRIVATE UrlLib
    PRIVATE base-n)

if(BGFX_BUILD_TOOLS AND BGFX_BUILD_TOOLS_SHADER)
    if(NOT IOS AND NOT VISIONOS AND NOT ANDROID)
        foreach(SHADER ${SHADERS})
            add_bgfx_shader(${SHADER} "")
        endforeach()
    endif()
    add_dependencies(Canvas shaderc)
endif()

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("3rd party Sources" ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FONT_SOURCES} ${NANOVG_SOURCES} ${ATLAS_SOURCES})
target_compile_definitions(Canvas PRIVATE _CRT_SECURE_NO_WARNINGS)
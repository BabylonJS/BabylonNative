include(${CMAKE_CURRENT_LIST_DIR}/shaderc.cmake)

if(NOT IOS AND NOT VISIONOS AND NOT ANDROID)
    set(SHADERC_PATH "" CACHE FILEPATH "Optional full path to shaderc built from bgfx.")
endif()

set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Colors.h"
    "Source/FrameBufferPool.cpp"
    "Source/FrameBufferPool.h"
    "Source/Image.cpp"
    "Source/Image.h"
    "Source/ImageData.cpp"
    "Source/ImageData.h"
    "Source/Path2D.cpp"
    "Source/Path2D.h"
    "Source/LineCaps.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    "Source/Gradient.cpp"
    "Source/Gradient.h"
    "Source/Font.cpp"
    "Source/Font.h"
    "Source/nanosvg.h"
    "Source/nanovg/nanovg.cpp"
    "Source/nanovg/nanovg.h"
    "Source/nanovg/nanovg_babylon.cpp"
    "Source/nanovg/nanovg_babylon.h"
    "Source/nanovg/nanovg_filterstack.cpp"
    "Source/nanovg/nanovg_filterstack.h"
    )

file(GLOB SHADERS "Source/Shaders/*.sc" "Source/Shaders/*.sh")

add_library(Canvas ${SOURCES} ${SHADERS})

target_include_directories(Canvas
    PUBLIC "Include"
    PRIVATE "Source"
    PRIVATE "${BGFX_DIR}/3rdparty")

target_link_libraries(Canvas
    PUBLIC napi
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext
    PRIVATE UrlLib
    PRIVATE base-n)

if(SHADERC_PATH AND NOT IOS AND NOT VISIONOS AND NOT ANDROID)
    if(EXISTS "${SHADERC_PATH}")
        message(STATUS "Using shaderc from: ${SHADERC_PATH}")
    else()
        message(FATAL_ERROR " '${SHADERC_PATH}' does not exist.")
    endif()

    foreach(SHADER ${SHADERS})
        add_bgfx_shader(${SHADER} "")
    endforeach()
endif()

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
source_group("3rd party Sources" ${CMAKE_CURRENT_SOURCE_DIR} FILES ${FONT_SOURCES} ${ATLAS_SOURCES})
source_group("Shaders" ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SHADERS})
target_compile_definitions(Canvas PRIVATE _CRT_SECURE_NO_WARNINGS)

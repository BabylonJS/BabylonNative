set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/Colors.h"
    "Source/Font.cpp"
    "Source/Font.h"
    "Source/Gradient.cpp"
    "Source/Gradient.h"
    "Source/Image.cpp"
    "Source/Image.h"
    "Source/ImageData.cpp"
    "Source/ImageData.h"
    "Source/LineCaps.h"
    "Source/MeasureText.cpp"
    "Source/MeasureText.h"
    "Source/Path2D.cpp"
    "Source/Path2D.h"
    "Source/Context.cpp"
    "Source/Context.h"
    "Source/nanosvg.h"
    # CanvasWgpu keeps the historical nvg* C API names for incremental
    # migration. These headers declare Babylon's compatibility ABI implemented
    # by Rust/femtovg (not upstream NanoVG and not bgfx).
    "Source/nanovg/nanovg.h"
    "Source/nanovg/nanovg_filterstack.h")

if(NOT TARGET babylon_graphics_backend)
    message(FATAL_ERROR "CanvasWgpu requires the GraphicsWgpu Rust backend target (babylon_graphics_backend).")
endif()

add_library(babylon_canvas_wgpu_backend INTERFACE)
target_link_libraries(babylon_canvas_wgpu_backend INTERFACE babylon_graphics_backend)

add_library(Canvas ${SOURCES})
warnings_as_errors(Canvas)
target_compile_options(Canvas PRIVATE
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wno-unused-parameter -Wno-unused-variable>)
target_compile_definitions(Canvas PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:_CRT_SECURE_NO_WARNINGS>)

target_include_directories(Canvas
    PUBLIC "Include"
    PRIVATE "Source")

target_link_libraries(Canvas
    PUBLIC napi
    PRIVATE base-n
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext
    PRIVATE UrlLib
    PRIVATE babylon_canvas_wgpu_backend)

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

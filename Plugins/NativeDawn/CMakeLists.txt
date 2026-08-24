set(SOURCES
    "Include/Babylon/Plugins/NativeDawn.h"
    "Source/CanvasDawn.cpp"
    "Source/CanvasDawn.h"
    "Source/ContextDawn.cpp"
    "Source/ContextDawn.h"
    "Source/ImageDataDawn.cpp"
    "Source/ImageDataDawn.h"
    "Source/ImageDecode.cpp"
    "Source/MeasureTextDawn.cpp"
    "Source/MeasureTextDawn.h"
    "Source/NativeDawn.cpp")

add_library(NativeDawn ${SOURCES})

target_include_directories(NativeDawn
    PUBLIC "Include")

target_link_libraries(NativeDawn
    PUBLIC napi
    PRIVATE JsRuntime
    PRIVATE webgpu_dawn
    PRIVATE Canvas
    # Image.h (reused from the Canvas polyfill) includes Babylon/Graphics headers.
    # Only the declarations are needed; no bgfx device is created on this path.
    PRIVATE GraphicsDeviceContext
    # Image.h also pulls in UrlLib (image loading from URLs).
    PRIVATE UrlLib
    PRIVATE bx
    PRIVATE bimg
    PRIVATE bimg_decode
    PRIVATE bimg_encode)

# NativeDawn drives the Dawn NanoVG backend directly (nanovg/nanovg_dawn.h) to
# implement a real 2D canvas context, and reuses the Canvas polyfill's
# backend-agnostic helpers (paths, gradients, images, fonts, colour parsing),
# so it needs Canvas's private source dir and bgfx's 3rdparty headers (stb).
target_include_directories(NativeDawn
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../Polyfills/Canvas/Source"
    PRIVATE "${BGFX_DIR}/3rdparty")

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    target_compile_definitions(NativeDawn
        PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
endif()

# NativeDawn exposes every WebGPU object as an N-API External wrapper. On the V8
# engine, jsruntimehost's N-API never drains its GC finalizer queue at runtime, so
# those wrappers (and the wgpu handles they hold) accumulate until the process OOMs.
# When building against V8, give the plugin access to the internal env header so it
# can drain the queue itself (see PumpJsFinalizers in NativeDawn.cpp). Other engines
# finalize through their own mechanisms and need none of this.
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_compile_definitions(NativeDawn PRIVATE NATIVEDAWN_V8_FINALIZER_DRAIN)
    target_include_directories(NativeDawn PRIVATE
        $<TARGET_PROPERTY:napi,INCLUDE_DIRECTORIES>
        "${JsRuntimeHost_SOURCE_DIR}/Core/Node-API/Source")
endif()

if(WIN32)
    target_compile_definitions(NativeDawn
        PRIVATE NOMINMAX)
endif()

set_property(TARGET NativeDawn PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

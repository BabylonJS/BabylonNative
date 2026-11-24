# NOTE: NativeWebGPU is transient and will be replaced in the near future.
# Do not base any development on this feature.
if(NOT TARGET Blob OR NOT TARGET URL)
    message(FATAL_ERROR "NativeWebGPU requires the JsRuntimeHost Blob and URL polyfills.")
endif()

set(SOURCES
    "Include/Babylon/Plugins/NativeWebGPU.h"
    "Source/CanvasDawn.cpp"
    "Source/CanvasDawn.h"
    "Source/ContextDawn.cpp"
    "Source/ContextDawn.h"
    "Source/ImageDataDawn.cpp"
    "Source/ImageDataDawn.h"
    "Source/ImageDecode.cpp"
    "Source/NativeWebGPU.cpp")

add_library(NativeWebGPU ${SOURCES})

target_compile_definitions(NativeWebGPU PRIVATE
    NATIVEWEBGPU_JAVASCRIPT_ENGINE="${NAPI_JAVASCRIPT_ENGINE}"
    NATIVEWEBGPU_PLATFORM="${BABYLON_NATIVE_PLATFORM}")

target_include_directories(NativeWebGPU
    PUBLIC "Include")

if(BABYLON_NATIVE_PLUGIN_TESTUTILS)
    target_compile_definitions(NativeWebGPU PRIVATE BABYLON_NATIVE_PLUGIN_TESTUTILS=1)
    target_link_libraries(NativeWebGPU PRIVATE TestUtils)
endif()

target_link_libraries(NativeWebGPU
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

# NativeWebGPU drives the Dawn NanoVG backend directly (nanovg/nanovg_dawn.h) to
# implement a real 2D canvas context, and reuses the Canvas polyfill's
# backend-agnostic helpers (paths, gradients, images, fonts, colour parsing),
# so it needs Canvas's private source dir and bgfx's 3rdparty headers (stb).
target_include_directories(NativeWebGPU
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../Polyfills/Canvas/Source"
    PRIVATE "${BGFX_DIR}/3rdparty")

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    target_compile_definitions(NativeWebGPU
        PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
endif()

if(WIN32)
    target_compile_definitions(NativeWebGPU
        PRIVATE NOMINMAX)
endif()

set_property(TARGET NativeWebGPU PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

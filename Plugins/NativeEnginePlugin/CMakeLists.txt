if(APPLE)
    set(GRAPHICS_API "Metal")
    set(XR OFF)
elseif(ANDROID)
    set(GRAPHICS_API "OpenGL")
    set(XR OFF)
elseif(WIN32)
    set(GRAPHICS_API "D3D")
    set(XR ON)
else()
    message(FATAL_ERROR "Unrecognized platform: graphics API could not be deduced")
endif()

set(SOURCES
    "Include/Babylon/NativeEnginePlugin.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/NativeEnginePlugin.cpp"
    "Source/NativeEngine.cpp"
    "Source/NativeEngine.h"
    "Source/ResourceLimits.cpp"
    "Source/ResourceLimits.h"
    "Source/ShaderCompiler.cpp"
    "Source/ShaderCompiler${GRAPHICS_API}.cpp"
    "Source/ShaderCompiler.h")

if(XR)
    set(SOURCES ${SOURCES}
        "Source/NativeXr.cpp"
        "Source/NativeXr.h")
endif()

add_library(NativeEnginePlugin ${SOURCES})

target_include_directories(NativeEnginePlugin PUBLIC "Include")

target_link_to_dependencies(NativeEnginePlugin
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE glslang
    PRIVATE SPIRV
    PRIVATE spirv-cross-hlsl
    PRIVATE WindowPolyfillImplementation)

if(XR)
    target_link_to_dependencies(NativeEnginePlugin
        PRIVATE xr)
    target_compile_definitions(NativeEnginePlugin PRIVATE NATIVE_ENGINE_XR)
endif()

if(APPLE)
    target_link_to_dependencies(NativeEnginePlugin
        PRIVATE spirv-cross-msl)
elseif(ANDROID)
    # Nothing to do.
elseif(WIN32)
    target_link_to_dependencies(NativeEnginePlugin
        PRIVATE "d3dcompiler.lib")
else()
    message(FATAL_ERROR "Unrecognized platform: graphics linking could not be performed")
endif()

set_property(TARGET NativeEnginePlugin PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

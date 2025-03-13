set(SOURCES
    "Include/Babylon/ShaderCache.h"
    "Include/Babylon/Plugins/NativeEngine.h"
    "Source/IndexBuffer.cpp"
    "Source/IndexBuffer.h"
    "Source/NativeDataStream.h"
    "Source/NativeEngineAPI.cpp"
    "Source/NativeEngine.cpp"
    "Source/NativeEngine.h"
    "Source/PerFrameValue.h"
    "Source/ShaderCompiler.h"
    "Source/ShaderCompilerCommon.h"
    "Source/ShaderCompilerCommon.cpp"
    "Source/ShaderCache.cpp"
    "Source/ShaderCache.h"
    "Source/ShaderCompilerTraversers.cpp"
    "Source/ShaderCompilerTraversers.h"
    "Source/ShaderCompiler${GRAPHICS_API}.cpp"
    "Source/VertexArray.cpp"
    "Source/VertexArray.h"
    "Source/VertexBuffer.cpp"
    "Source/VertexBuffer.h"
    "Source/JsConsoleLogger.h"
    "Source/JsConsoleLogger.cpp")

if(WIN32)
    set(SOURCES ${SOURCES} "Source/ShaderCompilerD3D.h")
endif()

add_library(NativeEngine ${SOURCES})

target_include_directories(NativeEngine
    PUBLIC "Include"
    PRIVATE "${BIMG_DIR}/3rdparty")


if(BGFX_BUILD_TOOLS)
    target_link_libraries(NativeEngine
        PUBLIC napi
        PRIVATE JsRuntime
        PRIVATE GraphicsDevice
        PRIVATE arcana
        PRIVATE bgfx
        PRIVATE bimg
        PRIVATE bimg_encode
        PRIVATE bimg_decode
        PRIVATE bx
        PRIVATE glslang
        PRIVATE spirv-opt
        PRIVATE GraphicsDeviceContext)
    target_compile_definitions(NativeEngine PRIVATE BGFX_BUILD_TOOLS)
else()
    target_link_libraries(NativeEngine
        PUBLIC napi
        PRIVATE JsRuntime
        PRIVATE GraphicsDevice
        PRIVATE arcana
        PRIVATE bgfx
        PRIVATE bimg
        PRIVATE bimg_encode
        PRIVATE bimg_decode
        PRIVATE bx
        PRIVATE glslang
        PRIVATE glslang-default-resource-limits
        PRIVATE SPIRV
        PRIVATE GraphicsDeviceContext)

    warnings_as_errors(NativeEngine)
endif()

if(TARGET spirv-cross-hlsl)
    target_link_libraries(NativeEngine
        PRIVATE spirv-cross-hlsl)
elseif(TARGET spirv-cross-msl)
    target_link_libraries(NativeEngine
        PRIVATE spirv-cross-msl)
elseif(TARGET spirv-cross-glsl)
    target_link_libraries(NativeEngine
        PRIVATE spirv-cross-glsl)
endif()

if(WIN32)
    target_link_libraries(NativeEngine
        PRIVATE "d3dcompiler.lib")
endif()

target_compile_definitions(NativeEngine
    PRIVATE NOMINMAX)

# TODO: remove this once the #define in ShaderCompilerCommon gets split into separate compilation units
target_compile_definitions(NativeEngine
    PRIVATE $<UPPER_CASE:${GRAPHICS_API}>)

set_property(TARGET NativeEngine PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

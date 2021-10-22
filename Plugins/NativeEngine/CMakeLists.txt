set(SOURCES
    "Include/Babylon/Plugins/NativeEngine.h"
    "Source/IndexBuffer.cpp"
    "Source/IndexBuffer.h"
    "Source/NativeDataStream.h"
    "Source/NativeEngineAPI.cpp"
    "Source/NativeEngine.cpp"
    "Source/NativeEngine.h"
    "Source/PerFrameValue.h"
    "Source/ResourceLimits.cpp"
    "Source/ResourceLimits.h"
    "Source/ShaderCompiler.h"
    "Source/ShaderCompilerCommon.h"
    "Source/ShaderCompilerCommon.cpp"
    "Source/ShaderCompilerTraversers.cpp"
    "Source/ShaderCompilerTraversers.h"
    "Source/ShaderCompiler${GRAPHICS_API}.cpp"
    "Source/Texture.cpp"
    "Source/Texture.h"
    "Source/VertexArray.cpp"
    "Source/VertexArray.h"
    "Source/VertexBuffer.cpp"
    "Source/VertexBuffer.h")

add_library(NativeEngine ${SOURCES})

target_include_directories(NativeEngine PUBLIC "Include"
    PRIVATE "${BIMG_DIR}/3rdparty") # for stb

target_link_to_dependencies(NativeEngine
    PUBLIC JsRuntime
    INTERFACE Graphics
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE glslang
    PRIVATE SPIRV
    PRIVATE spirv-cross-hlsl
    PRIVATE GraphicsInternal
    PRIVATE napi_extensions)
warnings_as_errors(NativeEngine)

if(APPLE)
    target_link_to_dependencies(NativeEngine
        PRIVATE spirv-cross-msl)
elseif(WIN32)
    target_link_to_dependencies(NativeEngine
        PRIVATE "d3dcompiler.lib")
endif()

target_compile_definitions(NativeEngine
    PRIVATE NOMINMAX)
target_compile_definitions(NativeEngine
    PRIVATE API${GRAPHICS_API}) # OpenGL is defined in bgfx.h. Using APIXXX instead

set_property(TARGET NativeEngine PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(NativeEngineInternal INTERFACE)
target_include_directories(NativeEngineInternal INTERFACE "Source")
target_link_to_dependencies(NativeEngineInternal
    INTERFACE NativeEngine
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx
    INTERFACE glslang
    INTERFACE SPIRV
    INTERFACE spirv-cross-hlsl
    INTERFACE GraphicsInternal
    INTERFACE napi_extensions)

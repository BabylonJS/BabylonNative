set(SOURCES
    "InternalInclude/Babylon/Plugins/ShaderCompiler.h"
    "Source/ShaderCompilerCommon.h"
    "Source/ShaderCompilerCommon.cpp"
    "Source/ShaderCompilerTraversers.cpp"
    "Source/ShaderCompilerTraversers.h")

if(GRAPHICS_API STREQUAL "D3D11" OR GRAPHICS_API STREQUAL "D3D12")
    set(SOURCES ${SOURCES} "Source/ShaderCompilerD3D.cpp")
else()
    set(SOURCES ${SOURCES} "Source/ShaderCompiler${GRAPHICS_API}.cpp")
endif()

add_library(ShaderCompiler ${SOURCES})

warnings_as_errors(ShaderCompiler)

target_include_directories(ShaderCompiler
    PRIVATE "InternalInclude")

target_link_libraries(ShaderCompiler
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bx
    PRIVATE glslang
    PRIVATE glslang-default-resource-limits
    PRIVATE GraphicsDeviceContext
    PRIVATE SPIRV)

if(TARGET spirv-cross-hlsl)
    target_link_libraries(ShaderCompiler
        PRIVATE spirv-cross-hlsl)
elseif(TARGET spirv-cross-msl)
    target_link_libraries(ShaderCompiler
        PRIVATE spirv-cross-msl)
elseif(TARGET spirv-cross-glsl)
    target_link_libraries(ShaderCompiler
        PRIVATE spirv-cross-glsl)
endif()

if(WIN32)
    target_link_libraries(ShaderCompiler
        PRIVATE "d3dcompiler.lib")
endif()

# TODO: remove this once the #define in ShaderCompilerCommon gets split into separate compilation units
target_compile_definitions(ShaderCompiler
    PRIVATE $<UPPER_CASE:${GRAPHICS_API}>)

set_property(TARGET ShaderCompiler PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(ShaderCompilerInternal INTERFACE)
target_include_directories(ShaderCompilerInternal
    INTERFACE "InternalInclude")
target_link_libraries(ShaderCompilerInternal
    INTERFACE ShaderCompiler
    INTERFACE GraphicsDeviceContext)

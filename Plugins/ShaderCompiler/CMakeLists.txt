set(SOURCES
    "InternalInclude/Babylon/Plugins/ShaderCompiler.h"
    "Source/ShaderCompilerCommon.h"
    "Source/ShaderCompilerCommon.cpp"
    "Source/ShaderCompilerTraversers.cpp"
    "Source/ShaderCompilerTraversers.h")

if(GRAPHICS_API STREQUAL "D3D11")
    set(SOURCES ${SOURCES} "Source/ShaderCompilerDXBC.cpp")
elseif(GRAPHICS_API STREQUAL "D3D12")
    set(SOURCES ${SOURCES} "Source/ShaderCompilerDXIL.cpp")
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

if(GRAPHICS_API STREQUAL "D3D11")
    target_link_libraries(ShaderCompiler
        PRIVATE "d3dcompiler.lib")
endif()

function(deploy_shader_compiler_runtime TARGET_NAME)
    if(WIN32 AND GRAPHICS_API STREQUAL "D3D12")
        if(CMAKE_VS_PLATFORM_NAME)
            set(DXC_TARGET_ARCHITECTURE "${CMAKE_VS_PLATFORM_NAME}")
        elseif(CMAKE_CXX_COMPILER_ARCHITECTURE_ID)
            set(DXC_TARGET_ARCHITECTURE "${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
        else()
            set(DXC_TARGET_ARCHITECTURE "${CMAKE_SYSTEM_PROCESSOR}")
        endif()
        string(TOLOWER "${DXC_TARGET_ARCHITECTURE}" DXC_TARGET_ARCHITECTURE_LOWER)

        if(WINDOWS_STORE)
            set(DXC_RUNTIME_SKIP_REASON "UWP targets require platform-specific packaging")
        elseif(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
            set(DXC_RUNTIME_SKIP_REASON "the target uses ${CMAKE_SIZEOF_VOID_P}-byte pointers")
        elseif(NOT DXC_TARGET_ARCHITECTURE_LOWER MATCHES "^(x64|amd64|x86_64)$")
            set(DXC_RUNTIME_SKIP_REASON "the target architecture is '${DXC_TARGET_ARCHITECTURE}'")
        endif()

        if(DXC_RUNTIME_SKIP_REASON)
            get_property(DXC_RUNTIME_WARNING_EMITTED GLOBAL PROPERTY BABYLON_DXC_RUNTIME_WARNING_EMITTED)
            if(NOT DXC_RUNTIME_WARNING_EMITTED)
                message(WARNING
                    "The bundled bgfx DirectX Shader Compiler runtime is x64 desktop-only; "
                    "automatic deployment is disabled because ${DXC_RUNTIME_SKIP_REASON}. "
                    "Provide a compatible dxcompiler.dll and dxil.dll through the platform's existing deployment mechanism.")
                set_property(GLOBAL PROPERTY BABYLON_DXC_RUNTIME_WARNING_EMITTED TRUE)
            endif()
            return()
        endif()

        set(DXC_RUNTIME_DIR "${BGFX_DIR}/tools/bin/windows")
        set(DXC_RUNTIME_FILES
            "${DXC_RUNTIME_DIR}/dxcompiler.dll"
            "${DXC_RUNTIME_DIR}/dxil.dll")

        foreach(DXC_RUNTIME_FILE IN LISTS DXC_RUNTIME_FILES)
            if(NOT EXISTS "${DXC_RUNTIME_FILE}")
                message(FATAL_ERROR "D3D12 shader compilation requires ${DXC_RUNTIME_FILE}")
            endif()
        endforeach()

        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${DXC_RUNTIME_FILES}
                "$<TARGET_FILE_DIR:${TARGET_NAME}>"
            COMMENT "Copying DirectX Shader Compiler runtime to ${TARGET_NAME}")
    endif()
endfunction()

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

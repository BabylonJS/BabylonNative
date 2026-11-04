include(${CMAKE_CURRENT_LIST_DIR}/../../Dependencies/shaderc.cmake)

set(SOURCES
    "Include/Babylon/Plugins/NativeEngine.h"
    "Source/IndexBuffer.cpp"
    "Source/IndexBuffer.h"
    "Source/InstanceRepacker.cpp"
    "Source/InstanceRepacker.h"
    "Source/JsConsoleLogger.h"
    "Source/JsConsoleLogger.cpp"
    "Source/NativeDataStream.h"
    "Source/NativeEngineAPI.cpp"
    "Source/NativeEngine.cpp"
    "Source/NativeEngine.h"
    "Source/PerFrameValue.h"
    "Source/PrimitiveModeExpansion.cpp"
    "Source/PrimitiveModeExpansion.h"
    "Source/Program.cpp"
    "Source/Program.h"
    "Source/ShaderProvider.h"
    "Source/ShaderProvider.cpp"
    "Source/StorageBuffer.cpp"
    "Source/StorageBuffer.h"
    "Source/VertexArray.cpp"
    "Source/VertexArray.h"
    "Source/VertexBuffer.cpp"
    "Source/VertexBuffer.h")

set(SHADERS "Source/Shaders/cs_instance_repack.sc")
foreach(BACKEND dxbc dxil essl glsl metal spirv)
    list(APPEND SHADERS "Source/Shaders/${BACKEND}/cs_instance_repack.h")
endforeach()

add_library(NativeEngine ${SOURCES} ${SHADERS})

if(SHADERC_PATH AND NOT IOS AND NOT VISIONOS AND NOT ANDROID)
    add_bgfx_shader("${CMAKE_CURRENT_SOURCE_DIR}/Source/Shaders/cs_instance_repack.sc" "")
endif()

warnings_as_errors(NativeEngine)

target_include_directories(NativeEngine
    PUBLIC "Include"
    PRIVATE "${BIMG_DIR}/3rdparty")

target_link_libraries(NativeEngine
    PUBLIC napi
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext
    PRIVATE JsRuntime)

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    target_compile_definitions(NativeEngine
        PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    target_link_libraries(NativeEngine
        PRIVATE bimg_encode
        PRIVATE bimg_decode)
endif()

if(BABYLON_NATIVE_PLUGIN_SHADERCACHE)
    target_compile_definitions(NativeEngine
        PRIVATE SHADER_CACHE)
    target_link_libraries(NativeEngine
        PRIVATE ShaderCacheInternal
        PRIVATE ShaderCache)
endif()

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_COMPILESHADERS)
    if(BABYLON_NATIVE_PLUGIN_SHADERCOMPILER)
        target_compile_definitions(NativeEngine
            PRIVATE SHADER_COMPILER)
        target_link_libraries(NativeEngine
            PRIVATE ShaderCompilerInternal)
    else()
        message(WARNING "Compiling shaders in NativeEngine plugin requires ShaderCompiler plugin to be enabled.")
    endif()
endif()

if(WIN32)
    target_compile_definitions(NativeEngine
        PRIVATE NOMINMAX)

    # arcana's ETW tracing requires a Vista-or-newer API baseline. Define it before
    # bx/platform.h chooses an older value for 32-bit and ARM64 builds.
    target_compile_definitions(NativeEngine
        PRIVATE _WIN32_WINNT=0x0A00 WINVER=0x0A00)
endif()

# Required by VertexBuffer::BuildInstanceDataBuffer. Remove once we have a better solution for that method.
target_compile_definitions(NativeEngine
    PRIVATE $<UPPER_CASE:${GRAPHICS_API}>)

# Expose test-only hooks (used by Apps/UnitTests) when building apps; excluded from consumer builds.
if(BABYLON_NATIVE_BUILD_APPS)
    target_compile_definitions(NativeEngine
        PUBLIC BABYLON_NATIVE_NATIVEENGINE_TEST_HOOKS)
endif()

set_property(TARGET NativeEngine PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Plugins/NativeEngine.h"
    "Source/IndexBuffer.cpp"
    "Source/IndexBuffer.h"
    "Source/JsConsoleLogger.h"
    "Source/JsConsoleLogger.cpp"
    "Source/NativeDataStream.h"
    "Source/NativeEngineAPI.cpp"
    "Source/NativeEngine.cpp"
    "Source/NativeEngine.h"
    "Source/PerFrameValue.h"
    "Source/Program.cpp"
    "Source/Program.h"
    "Source/ShaderProvider.h"
    "Source/ShaderProvider.cpp"
    "Source/VertexArray.cpp"
    "Source/VertexArray.h"
    "Source/VertexBuffer.cpp"
    "Source/VertexBuffer.h")

add_library(NativeEngine ${SOURCES})

warnings_as_errors(NativeEngine)

target_include_directories(NativeEngine
    PUBLIC "Include"
    PRIVATE "${BIMG_DIR}/3rdparty")

target_link_libraries(NativeEngine
    PUBLIC napi
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bimg_encode
    PRIVATE bimg_decode
    PRIVATE bx
    PRIVATE minz
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext
    PRIVATE JsRuntime)

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP)
    target_compile_definitions(NativeEngine
        PRIVATE WEBP)
    target_link_libraries(NativeEngine
        PRIVATE webp)
endif()

if(BABYLON_NATIVE_PLUGIN_SHADERCACHE)
    target_compile_definitions(NativeEngine
        PRIVATE SHADER_CACHE)
    target_link_libraries(NativeEngine
        PRIVATE ShaderCacheInternal)
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
endif()

# Required by VertexBuffer::BuildInstanceDataBuffer. Remove once we have a better solution for that method.
target_compile_definitions(NativeEngine
    PRIVATE $<UPPER_CASE:${GRAPHICS_API}>)

set_property(TARGET NativeEngine PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

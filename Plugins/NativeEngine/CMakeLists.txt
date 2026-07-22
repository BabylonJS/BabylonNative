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
    PRIVATE bx
    PRIVATE minz
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext
    PRIVATE JsRuntime)

if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    target_compile_definitions(NativeEngine
        PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    target_link_libraries(NativeEngine
        PRIVATE bimg
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

    # arcana's Windows trace_region.h (pulled in by NativeEngine.cpp and Program.cpp)
    # emits ETW TraceLogging events. The EventRegister/EventUnregister/EventWriteTransfer
    # declarations it relies on (via <evntprov.h>) are only visible when
    # _WIN32_WINNT >= _WIN32_WINNT_VISTA (0x0600). bx/platform.h only sets _WIN32_WINNT
    # when it is still undefined, and then to 0x0601 for 64-bit but 0x0502 (pre-Vista)
    # for 32-bit; its 64-bit check also misses MSVC's _M_ARM64, so Windows ARM64 is
    # treated as 32-bit. Both cases leave _WIN32_WINNT below Vista and break the compile
    # (error C3861). Pin a modern Windows API baseline so it is set before any header.
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

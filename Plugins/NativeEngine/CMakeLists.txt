if(WIN32 OR WINDOWS_STORE)
    set(GRAPHICS_API "D3D")
else()
    message(FATAL_ERROR "Unrecognized platform: graphics API could not be deduced")
endif()

set(SOURCES
    "Include/Babylon/NativeEngine.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/InitializeNativeEngine.cpp"
    "Source/NativeEngine.cpp"
    "Source/NativeEngine.h"
    "Source/NativeXr.cpp"
    "Source/NativeXr.h"
    "Source/ResourceLimits.cpp"
    "Source/ResourceLimits.h"
    "Source/ShaderCompiler.cpp"
    "Source/ShaderCompiler${GRAPHICS_API}.cpp"
    "Source/ShaderCompiler.h")

add_library(NativeEngine ${SOURCES})

target_include_directories(NativeEngine PUBLIC "Include")

target_link_libraries(NativeEngine
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx
    PRIVATE glslang
    PRIVATE SPIRV
    PRIVATE spirv-cross-hlsl
    PRIVATE NativeWindow
    PRIVATE xr)

if(WIN32 OR WINDOWS_STORE)
    target_link_libraries(NativeEngine
        #PRIVATE "d3d11.lib"
        PRIVATE "d3dcompiler.lib")
else()
    message(FATAL_ERROR "Unrecognized platform: graphics linking could not be performed")
endif()

set_property(TARGET NativeEngine PROPERTY FOLDER Plugins/Babylon)

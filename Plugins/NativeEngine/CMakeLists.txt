if(WIN32 OR WINDOWS_STORE)
    set(GRAPHICS_API "D3D")
else()
    message(FATAL_ERROR "Unrecognized platform: graphics API could not be deduced")
endif()

set(SOURCES
    "Include/Babylon/NativeEngine.h"
    "Include/Babylon/NativeXr.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/NativeEngine.cpp"
    "Source/NativeXr.cpp"
    "Source/ResourceLimits.cpp"
    "Source/ResourceLimits.h"
    "Source/ShaderCompiler${GRAPHICS_API}.cpp"
    "Source/ShaderCompiler.h")

add_library(NativeEngine ${SOURCES})

target_include_directories(NativeEngine PRIVATE "Include/Babylon")
target_include_directories(NativeEngine INTERFACE "Include")

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

set_property(TARGET NativeEngine PROPERTY FOLDER Plugins/Babylon)

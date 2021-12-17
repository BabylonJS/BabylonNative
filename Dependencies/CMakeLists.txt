# -------------------------------- CMakeExtensions --------------------------------
add_subdirectory(CMakeExtensions)

# -------------------------------- arcana --------------------------------
# Dependencies: none
add_subdirectory(arcana.cpp)
if(WIN32)
    set_property(TARGET arcana PROPERTY FOLDER Dependencies)
endif()

# -------------------------------- GSL -----------------------------------
# Dependencies: none
add_subdirectory(arcana.cpp/Source/Submodules/GSL)

# -------------------------------- base-n --------------------------------
# Dependencies: none
add_library(base-n INTERFACE)
target_include_directories(base-n INTERFACE "base-n/include")

# -------------------------------- bgfx.cmake --------------------------------
# Dependencies: none
set(BGFX_BUILD_EXAMPLES OFF CACHE BOOL "Build the BGFX examples.")
set(BGFX_BUILD_TOOLS OFF CACHE BOOL "Build the BGFX tools.")
set(BGFX_USE_DEBUG_SUFFIX OFF CACHE BOOL "Add 'd' suffix to debug output targets")
set(BGFX_OPENGL_USE_EGL ON CACHE BOOL "Use EGL targets for OpenGL")
add_subdirectory(bgfx.cmake)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEBUG_UNIFORM=0)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MULTITHREADED=1)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MAX_VERTEX_STREAMS=16)
target_compile_definitions(bgfx PRIVATE BGFX_GL_CONFIG_BLIT_EMULATION=1)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MAX_COMMAND_BUFFER_SIZE=12582912)
if(GRAPHICS_API STREQUAL "D3D11")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_DIRECT3D11=1)
elseif(GRAPHICS_API STREQUAL "D3D12")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_DIRECT3D12=1)
elseif(GRAPHICS_API STREQUAL "Metal")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_METAL=1)
elseif(GRAPHICS_API STREQUAL "OpenGL")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_OPENGLES=30)
    target_compile_definitions(bgfx PRIVATE BGFX_GL_CONFIG_TEXTURE_READ_BACK_EMULATION=1)
elseif(GRAPHICS_API STREQUAL "Vulkan")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_VULKAN=1)
endif()
set_property(TARGET astc PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET astc-codec PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET edtaa3 PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET etc1 PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET etc2 PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET iqa PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET nvtt PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET pvrtc PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET squish PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET bgfx PROPERTY FOLDER Dependencies/bgfx)
set_property(TARGET bimg PROPERTY FOLDER Dependencies/bgfx)
set_property(TARGET bx PROPERTY FOLDER Dependencies/bgfx)
disable_warnings(astc)
disable_warnings(astc-codec)
disable_warnings(edtaa3)
disable_warnings(etc1)
disable_warnings(etc2)
disable_warnings(iqa)
disable_warnings(nvtt)
disable_warnings(pvrtc)
disable_warnings(squish)
disable_warnings(bgfx)
disable_warnings(bimg)
disable_warnings(bx)

# -------------------------------- glslang --------------------------------
# Dependencies: none
set(SKIP_GLSLANG_INSTALL OFF CACHE BOOL "Skip installation")
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "Enables building of SPVRemapper")
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "Builds glslangValidator and spirv-remap")
set(ENABLE_HLSL OFF CACHE BOOL "Enables HLSL input support")
set(ENABLE_OPT OFF CACHE BOOL "Enables spirv-opt capability if present")
set(BUILD_EXTERNAL OFF CACHE BOOL "Build external dependencies in /External")
add_subdirectory(glslang)
set_property(TARGET GenericCodeGen PROPERTY FOLDER Dependencies/glslang)
set_property(TARGET glslang PROPERTY FOLDER Dependencies/glslang)
set_property(TARGET OGLCompiler PROPERTY FOLDER Dependencies/glslang)
set_property(TARGET OSDependent PROPERTY FOLDER Dependencies/glslang)
set_property(TARGET MachineIndependent PROPERTY FOLDER Dependencies/glslang)
set_property(TARGET SPIRV PROPERTY FOLDER Dependencies/glslang)
disable_warnings(GenericCodeGen)
disable_warnings(glslang)
disable_warnings(OGLCompiler)
disable_warnings(OSDependent)
disable_warnings(MachineIndependent)
disable_warnings(SPIRV)

# -------------------------------- ios-cmake --------------------------------
# Nothing to do here.

# -------------------------------- napi --------------------------------
# Dependencies: none
add_subdirectory(napi)
set_property(TARGET napi PROPERTY FOLDER Dependencies)

# -------------------------------- SPIRV-Cross --------------------------------
# Dependencies: none
set(SPIRV_CROSS_ENABLE_TESTS OFF CACHE BOOL "Enable SPIRV-Cross tests.")
add_subdirectory(SPIRV-Cross)
set_property(TARGET spirv-cross-core PROPERTY FOLDER Dependencies/SPIRV-Cross)
set_property(TARGET spirv-cross-glsl PROPERTY FOLDER Dependencies/SPIRV-Cross)
set_property(TARGET spirv-cross-hlsl PROPERTY FOLDER Dependencies/SPIRV-Cross)
disable_warnings(spirv-cross-core)
disable_warnings(spirv-cross-glsl)
disable_warnings(spirv-cross-hlsl)

# -------------------------------- xr --------------------------------
# Dependencies: none
# Currently supported on Windows via OpenXR, Android via ARCore, and iOS via ARKit.
if(WIN32 OR WINDOWS_STORE OR ANDROID OR IOS)
    add_subdirectory(xr)
    set_property(TARGET xr PROPERTY FOLDER Dependencies/xr)
    warnings_as_errors(xr)
    if(WIN32 OR WINDOWS_STORE)
        set_property(TARGET openxr_loader PROPERTY FOLDER Dependencies/xr/OpenXR)
        set_property(TARGET generate_openxr_header PROPERTY FOLDER Dependencies/xr/OpenXR/Generated)
        set_property(TARGET xr_global_generated_files PROPERTY FOLDER Dependencies/xr/OpenXR/Generated)
        disable_warnings(openxr_loader)
    endif()
endif()

# -------------------------------- AndroidExtensions --------------------------------
# Dependencies: none
if(ANDROID)
    add_subdirectory(AndroidExtensions)
endif()

# -------------------------------- UrlLib --------------------------------
# Dependencies: arcana, AndroidExtensions
add_subdirectory(UrlLib)
set_property(TARGET UrlLib PROPERTY FOLDER Dependencies)

# --------------------------- v8inspector ---------------------------
# Dependencies: none
# Currently only supported on Win32 x86/x64 and Android
if ((WIN32 AND NOT (WINDOWS_STORE OR CMAKE_GENERATOR_PLATFORM_UPPER MATCHES "ARM.*")) OR ANDROID)
    add_subdirectory(v8inspector)
    set_property(TARGET v8inspector PROPERTY FOLDER Dependencies)
endif()

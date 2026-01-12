set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

# --------------------------------------------------
# AndroidExtensions
# --------------------------------------------------
if(ANDROID OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(AndroidExtensions)
endif()

# --------------------------------------------------
# arcana.cpp
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(arcana.cpp)

# --------------------------------------------------
# arcore-android-sdk
# --------------------------------------------------
if(ANDROID OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(arcore-android-sdk)
endif()

# --------------------------------------------------
# base-n
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(base-n)

add_library(base-n INTERFACE)
target_include_directories(base-n INTERFACE "${base-n_SOURCE_DIR}/include")

# --------------------------------------------------
# bgfx.cmake
# --------------------------------------------------
set(BGFX_BUILD_EXAMPLES OFF)
set(BGFX_BUILD_TOOLS OFF)
set(BGFX_CONFIG_MULTITHREADED ON)
set(BGFX_CUSTOM_TARGETS OFF)
set(BGFX_INSTALL OFF)
set(BGFX_OPENGL_USE_EGL ON)
set(BGFX_USE_DEBUG_SUFFIX OFF)
FetchContent_MakeAvailable_With_Message(bgfx.cmake)

# Turn off debug annotations as it causes an access violation in D3D12.
# This flag is set using compile definitions because the bgfx.cmake option is ignored in debug configuration.
# See https://github.com/BabylonJS/bgfx.cmake/blob/0af3c9865a66aff1748a51bb466b24f05a123043/cmake/bgfx/bgfx.cmake#L126.
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEBUG_ANNOTATION=0)

target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_DEFAULT_MAX_ENCODERS=2)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MAX_VERTEX_STREAMS=18)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MIN_RESOURCE_COMMAND_BUFFER_SIZE=16)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_MIN_UNIFORM_BUFFER_SIZE=4096)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_UNIFORM_BUFFER_RESIZE_THRESHOLD_SIZE=256)
target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_UNIFORM_BUFFER_RESIZE_INCREMENT_SIZE=1024)
if(GRAPHICS_API STREQUAL "D3D11")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_DIRECT3D11=1)
elseif(GRAPHICS_API STREQUAL "D3D12")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_DIRECT3D12=1)
elseif(GRAPHICS_API STREQUAL "Metal")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_METAL=1)
elseif(GRAPHICS_API STREQUAL "OpenGL")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_OPENGLES=30)
    target_compile_definitions(bgfx PRIVATE BGFX_GL_CONFIG_BLIT_EMULATION=1)
    target_compile_definitions(bgfx PRIVATE BGFX_GL_CONFIG_TEXTURE_READ_BACK_EMULATION=1)
elseif(GRAPHICS_API STREQUAL "Vulkan")
    target_compile_definitions(bgfx PRIVATE BGFX_CONFIG_RENDERER_VULKAN=1)
endif()
set_property(TARGET bimg PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET bimg PROPERTY UNITY_BUILD false)
set_property(TARGET bimg_encode PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET bimg_encode PROPERTY UNITY_BUILD false)
set_property(TARGET bimg_decode PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET bimg_decode PROPERTY UNITY_BUILD false)
set_property(TARGET minz PROPERTY FOLDER Dependencies/bgfx/3rdparty)
set_property(TARGET minz PROPERTY UNITY_BUILD false)
if(TARGET tinyexr)
    set_property(TARGET tinyexr PROPERTY FOLDER Dependencies/bgfx/3rdparty)
endif()
set_property(TARGET bgfx PROPERTY FOLDER Dependencies/bgfx)
set_property(TARGET bimg PROPERTY FOLDER Dependencies/bgfx)
set_property(TARGET bx PROPERTY FOLDER Dependencies/bgfx)
target_compile_definitions(bx PRIVATE _CRT_SECURE_NO_WARNINGS)

if(APPLE)
    set_property(TARGET bgfx PROPERTY UNITY_BUILD false)
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    # Use GLVND libraries for EGL support in bgfx
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

# --------------------------------------------------
# glslang
# --------------------------------------------------
set(BUILD_EXTERNAL OFF)
set(ENABLE_SPVREMAPPER OFF)
set(ENABLE_GLSLANG_BINARIES OFF)
set(ENABLE_HLSL OFF)
set(ENABLE_OPT OFF)
set(ENABLE_CTEST OFF)
set(SKIP_GLSLANG_INSTALL ON)
set(ENABLE_GLSLANG_WEBMIN ON)
set(ENABLE_GLSLANG_WEBMIN_DEVEL OFF)

if(NOT TARGET glslang)
    FetchContent_MakeAvailable_With_Message(glslang)

    set_property(TARGET GenericCodeGen PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET glslang PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET glslang-default-resource-limits PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET OGLCompiler PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET OSDependent PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET MachineIndependent PROPERTY FOLDER Dependencies/glslang)
    set_property(TARGET SPIRV PROPERTY FOLDER Dependencies/glslang)
endif()

# --------------------------------------------------
# googletest
# --------------------------------------------------
if(BABYLON_NATIVE_BUILD_APPS AND (WIN32 OR (APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID AND NOT APPLE)))
    if(WIN32)
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        # Default build type for my test projects are /MDd (MultiThreaded DLL) but GTests default to /MTd (MultiTreaded)
        # see https://github.com/google/googletest/blob/main/googletest/README.md
        # "Enabling this option will make gtest link the runtimes dynamically too, and match the project in which it is included."
        set(gtest_force_shared_crt OFF)
    endif()

    FetchContent_MakeAvailable_With_Message(googletest)

    set_property(TARGET gmock PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gmock_main PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest_main PROPERTY FOLDER Dependencies/GoogleTest)
endif()

# --------------------------------------------------
# JsRuntimeHost
# --------------------------------------------------
FetchContent_MakeAvailable_With_Message(JsRuntimeHost)

set_property(TARGET JsRuntime PROPERTY UNITY_BUILD false)
if(TARGET AppRuntime)
    set_property(TARGET AppRuntime PROPERTY UNITY_BUILD false)
endif()

if(WIN32)
    # UrlLib does not support unity build on win32
    set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)
endif()

# --------------------------------------------------
# SPIRV-Cross
# --------------------------------------------------
set(SPIRV_CROSS_CLI OFF)
set(SPIRV_CROSS_ENABLE_TESTS OFF)
set(SPIRV_CROSS_ENABLE_CPP OFF)
set(SPIRV_CROSS_ENABLE_REFLECT OFF)
set(SPIRV_CROSS_ENABLE_C_API OFF)
set(SPIRV_CROSS_ENABLE_UTIL OFF)
set(SPIRV_CROSS_SKIP_INSTALL ON)
set(SPIRV_CROSS_ENABLE_WEBMIN ON)
if(NOT GRAPHICS_API STREQUAL "OpenGL")
    set(SPIRV_CROSS_ENABLE_GLSL OFF)
endif()
if(NOT GRAPHICS_API STREQUAL "Metal")
    set(SPIRV_CROSS_ENABLE_MSL OFF)
endif()
if(NOT GRAPHICS_API STREQUAL "D3D11" AND NOT GRAPHICS_API STREQUAL "D3D12")
    set(SPIRV_CROSS_ENABLE_HLSL OFF)
endif()
FetchContent_MakeAvailable_With_Message(SPIRV-Cross)
set_property(TARGET spirv-cross-core PROPERTY FOLDER Dependencies/SPIRV-Cross)
if(TARGET spirv-cross-glsl)
    set_property(TARGET spirv-cross-glsl PROPERTY FOLDER Dependencies/SPIRV-Cross)
endif()
if(TARGET spirv-cross-msl)
    set_property(TARGET spirv-cross-msl PROPERTY FOLDER Dependencies/SPIRV-Cross)
endif()
if(TARGET spirv-cross-hlsl)
    set_property(TARGET spirv-cross-hlsl PROPERTY FOLDER Dependencies/SPIRV-Cross)
endif()

# HACK: Re-enabled disabled warnings in SPIRV-Cross as they cause alerts in BinSkim
foreach(target spirv-cross-core spirv-cross-glsl spirv-cross-msl spirv-cross-hlsl)
    if(TARGET ${target})
        get_target_property(compileOptions ${target} COMPILE_OPTIONS)
        list(REMOVE_ITEM compileOptions /wd4267 /wd4996)
        set_target_properties(${target} PROPERTIES COMPILE_OPTIONS "${compileOptions}")
    endif()
endforeach()

# Disable warnings until unused parameter [-Wunused-parameter] get fixed.
if(TARGET SPIRV)
    disable_warnings(SPIRV)
endif()
if(TARGET MachineIndependent)
    disable_warnings(MachineIndependent)
endif()

# --------------------------------------------------
# xr
# --------------------------------------------------
# Currently supported on Android via ARCore, and iOS via ARKit.
if((BABYLON_NATIVE_PLUGIN_NATIVEXR AND (ANDROID OR IOS)) OR BABYLON_NATIVE_BUILD_SOURCETREE)
    add_subdirectory(xr)
    set_property(TARGET xr PROPERTY FOLDER Dependencies/xr)
    warnings_as_errors(xr)
endif()

# --------------------------------------------------
# WebP
# --------------------------------------------------
if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP)
    set(WEBP_BUILD_ANIM_UTILS OFF)
    set(WEBP_BUILD_CWEBP OFF)
    set(WEBP_BUILD_DWEBP OFF)
    set(WEBP_BUILD_GIF2WEBP OFF)
    set(WEBP_BUILD_IMG2WEBP OFF)
    set(WEBP_BUILD_VWEBP OFF)
    set(WEBP_BUILD_WEBPINFO OFF)
    set(WEBP_BUILD_LIBWEBPMUX OFF)
    set(WEBP_BUILD_WEBPMUX OFF)
    set(WEBP_BUILD_EXTRAS OFF)
    FetchContent_MakeAvailable_With_Message(libwebp)

    set_property(TARGET sharpyuv PROPERTY UNITY_BUILD false)
    set_property(TARGET sharpyuv PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webp PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webpdecode PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webpdecoder PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webpdemux PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webpdsp PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webpdspdecode PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webpencode PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webputils PROPERTY FOLDER Dependencies/libwebp)
    set_property(TARGET webputilsdecode PROPERTY FOLDER Dependencies/libwebp)
endif()

# --------------------------------------------------
# WindowsAppSDK
# --------------------------------------------------
if(WINDOWS_STORE)
    add_subdirectory(WindowsAppSDK)
endif()

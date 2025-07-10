cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
    GIT_TAG 7d88a601fda9892791e7b4e994e375e049615688)
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG 1a8a5d6e95413ed14b38a6ac9419048f9a9c8009)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976)
FetchContent_Declare(base-n
    GIT_REPOSITORY https://github.com/azawadzki/base-n.git
    GIT_TAG 7573e77c0b9b0e8a5fb63d96dbde212c921993b4)
FetchContent_Declare(bgfx.cmake
    GIT_REPOSITORY https://github.com/BabylonJS/bgfx.cmake.git
    GIT_TAG fdf3ed4a62acabd7025d0a843f6d8a9aff5e3612)
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/BabylonJS/glslang.git
    GIT_TAG 6388a301b23edf0ae6a9342d46efa9cb21f05d9b)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.14.0.tar.gz")
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.5.0)
FetchContent_Declare(JsRuntimeHost
    GIT_REPOSITORY https://github.com/BabylonJS/JsRuntimeHost.git
    GIT_TAG 8e6a71e78de6880d12b225cfbcc2cc6348c55e61)
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/BabylonJS/SPIRV-Cross.git
    GIT_TAG 6abfcf066d171e9ade7604d91381ebebe4209edc)
FetchContent_Declare(libwebp
    GIT_REPOSITORY https://github.com/webmproject/libwebp.git
    GIT_TAG 57e324e2eb99be46df46d77b65705e34a7ae616c)
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

if(VISIONOS)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "VISIONOSCOMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "1.0" CACHE STRING "")
    set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
elseif(IOS)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "13" CACHE STRING "")
    set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
endif()

project(BabylonNative)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Options
# --------------------------------------------------

# General
option(BABYLON_NATIVE_BUILD_APPS "Build Babylon Native apps." ${PROJECT_IS_TOP_LEVEL})
option(BABYLON_NATIVE_INSTALL "Include the install target." ${PROJECT_IS_TOP_LEVEL})
option(BABYLON_DEBUG_TRACE "Enable debug trace." OFF)

# WARNING: This is experimental. Only use it if you can ensure that your application will properly handle thread affinity.
option(BABYLON_NATIVE_CHECK_THREAD_AFFINITY "Checks thread safety in the graphics device calls. It can be removed if hosting application ensures thread coherence." ON)

# Plugins
option(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE "Include Babylon Native Plugin ExternalTexture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAMERA "Include Babylon Native Plugin NativeCamera." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE "Include Babylon Native Plugin NativeCapture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE "Include Babylon Native Plugin NativeEngine." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP "Include Babylon Native Plugin NativeEngine - WebP." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEINPUT "Include Babylon Native Plugin NativeInput." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS "Include Babylon Native Plugin NativeOptimizations." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVETRACING "Include Babylon Native Plugin NativeTracing." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEXR "Include Babylon Native Plugin XR." ON)
option(BABYLON_NATIVE_PLUGIN_TESTUTILS "Include Babylon Native Plugin TestUtils." ON)

# Polyfills
option(BABYLON_NATIVE_POLYFILL_WINDOW "Include Babylon Native Polyfill Window." ON)
option(BABYLON_NATIVE_POLYFILL_CANVAS "Include Babylon Native Polyfill Canvas." ON)

# --------------------------------------------------

if(ANDROID)
    set(BABYLON_NATIVE_PLATFORM "Android")
elseif(IOS)
    set(BABYLON_NATIVE_PLATFORM "iOS")
elseif (VISIONOS)
    set(BABYLON_NATIVE_PLATFORM "visionOS")
elseif(APPLE)
    set(BABYLON_NATIVE_PLATFORM "macOS")
elseif(WINDOWS_STORE)
    set(BABYLON_NATIVE_PLATFORM "WinRT")
elseif(WIN32)
    set(BABYLON_NATIVE_PLATFORM "Win32")
elseif(UNIX)
    set(BABYLON_NATIVE_PLATFORM "Unix")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(APPLE)
    set(GRAPHICS_API Metal)
elseif(UNIX)
    if(NOT GRAPHICS_API)
        set(GRAPHICS_API OpenGL)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan AND NOT GRAPHICS_API STREQUAL OpenGL)
            message(FATAL_ERROR "Unrecognized/Unsupported render API: ${GRAPHICS_API}")
        endif()
    endif()
elseif(WIN32)
    if(NOT GRAPHICS_API)
        set(GRAPHICS_API D3D11)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan AND NOT GRAPHICS_API STREQUAL D3D11 AND NOT GRAPHICS_API STREQUAL D3D12)
            message(FATAL_ERROR "Unrecognized/Unsupported render API: ${GRAPHICS_API}")
        endif()
    endif()
endif()

if(APPLE)
    set(BABYLON_NATIVE_PLATFORM_IMPL_EXT "mm")
else()
    set(BABYLON_NATIVE_PLATFORM_IMPL_EXT "cpp")
endif()

if(MSVC)
    # __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
    # https://gitlab.kitware.com/cmake/cmake/-/issues/18837
    add_compile_options(/Zc:__cplusplus)

    # https://devblogs.microsoft.com/cppblog/announcing-full-support-for-a-c-c-conformant-preprocessor-in-msvc/
    add_compile_options(/Zc:preprocessor)

    # default to /W3
    add_compile_options(/W3)
endif()

if(APPLE)
    # Without this option on azure pipelines, there is a mismatch with math.h giving a lot of undefined functions on macOS.
    # Only enabled for Apple as there is no issue for Windows.
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
    # Create scheme for installation and other targets
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
endif()

if(BABYLON_DEBUG_TRACE)
    add_compile_definitions(BABYLON_DEBUG_TRACE)
endif()

add_subdirectory(Dependencies)
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Polyfills)

if(BABYLON_NATIVE_BUILD_APPS)
    add_subdirectory(Apps)
endif()

# no install for glslang compiled for shaderc
if(BABYLON_NATIVE_INSTALL AND NOT BGFX_BUILD_TOOLS)
    include(Install/Install.cmake)
endif()

cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/bghgary/AndroidExtensions.git
    GIT_TAG 97ebfb5046ed4e553e306a75e5fafc9c6b72cb39)
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG f2757396e80bc4169f2ddb938ce25367a98ffdd0)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976)
FetchContent_Declare(base-n
    GIT_REPOSITORY https://github.com/azawadzki/base-n.git
    GIT_TAG 7573e77c0b9b0e8a5fb63d96dbde212c921993b4)
FetchContent_Declare(bgfx.cmake
    GIT_REPOSITORY https://github.com/BabylonJS/bgfx.cmake.git
    GIT_TAG 3274a0b5755756d32cef18ed1859a342c1b90abd)
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/bghgary/CMakeExtensions.git
    GIT_TAG 4d3ed5a16f3660ba0f144fb08b5021cf024d5d2d)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/BabylonJS/glslang.git
    GIT_TAG 81f27a4399562459191e0506c0c1c353d99ba6e4)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz")
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.4.1)
FetchContent_Declare(JsRuntimeHost
    GIT_REPOSITORY https://github.com/bghgary/JsRuntimeHost.git
    GIT_TAG 8ac284dba4df8fc271c6bf05813530007935d6c7)
FetchContent_Declare(OpenXR-MixedReality
    GIT_REPOSITORY https://github.com/microsoft/OpenXR-MixedReality.git
    GIT_TAG 67424511525b96a36847f2a96d689d99e5879503)
FetchContent_Declare(OpenXR-SDK
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
    GIT_TAG 458984d7f59d1ae6dc1b597d94b02e4f7132eaba)
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/KhronosGroup/SPIRV-Cross.git
    GIT_TAG 54997fb4bc3adeb47b9b9f7bb67f1c25eaca2204)
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

if(IOS)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "13" CACHE STRING "")
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

# Plugins
option(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE "Include Babylon Native Plugin ExternalTexture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAMERA "Include Babylon Native Plugin NativeCamera." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE "Include Babylon Native Plugin NativeCapture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE "Include Babylon Native Plugin NativeEngine." ON)
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

    # default to /W3
    add_compile_options(/W3)
endif()

if(APPLE)
    # Without this option on azure pipelines, there is a mismatch with math.h giving a lot of undefined functions on macOS.
    # Only enabled for Apple as there is no issue for Windows.
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
endif()

add_subdirectory(Dependencies)
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Polyfills)

if(BABYLON_NATIVE_BUILD_APPS)
    add_subdirectory(Apps)
endif()

if(BABYLON_NATIVE_INSTALL)
    include(Install/Install.cmake)
endif()

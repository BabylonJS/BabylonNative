cmake_minimum_required(VERSION 3.21)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set fetched URL contents to the time of extraction, not the timestamps in the archive.
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# Modules declaration
FetchContent_Declare(cmake-extensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG efe7101be5e04391b9e55f68a01d534f54d3026e)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.4.1)
FetchContent_Declare(arcana
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG f2757396e80bc4169f2ddb938ce25367a98ffdd0)
FetchContent_Declare(JsRuntimeHost
    GIT_REPOSITORY https://github.com/BabylonJS/JsRuntimeHost.git
    GIT_TAG 2518f6c4be108edfd23a9de856c996f1b2bc2f9f)
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
    GIT_TAG 1a47db416ec2aae3f51b28b94f73e8f54e412d0d)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz")
FetchContent_Declare(base-n
    GIT_REPOSITORY https://github.com/azawadzki/base-n.git
    GIT_TAG 7573e77c0b9b0e8a5fb63d96dbde212c921993b4)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/BabylonJS/glslang.git
    GIT_TAG 6388a301b23edf0ae6a9342d46efa9cb21f05d9b)
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/BabylonJS/SPIRV-Cross.git
    GIT_TAG 43e7999af1c9af7d3ed9d4fc3a44cf3e68235cfd)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976)
FetchContent_Declare(OpenXR-SDK
    GIT_REPOSITORY https://github.com/KhronosGroup/OpenXR-SDK.git
    GIT_TAG 458984d7f59d1ae6dc1b597d94b02e4f7132eaba)
FetchContent_Declare(OpenXR-MixedReality
    GIT_REPOSITORY https://github.com/microsoft/OpenXR-MixedReality.git
    GIT_TAG 67424511525b96a36847f2a96d689d99e5879503)
FetchContent_Declare(bgfx
    GIT_REPOSITORY https://github.com/BabylonJS/bgfx.cmake.git
    GIT_TAG 3274a0b5755756d32cef18ed1859a342c1b90abd)

set(CONTENT_TO_FETCH cmake-extensions arcana JsRuntimeHost base-n)

if(IOS)
    FetchContent_MakeAvailable(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "12" CACHE STRING "")
    set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
endif()

project(BabylonNative)

# --------------------------------------------------
# Options
# --------------------------------------------------
option(BABYLON_NATIVE_BUILD_APPS "Build Babylon Native apps." ${PROJECT_IS_TOP_LEVEL})
option(BABYLON_NATIVE_INSTALL "Include the install target." ${PROJECT_IS_TOP_LEVEL})
option(BABYLON_NATIVE_MSVC_SECURITY_FLAGS "Enable MSVC secuirity flags", OFF)

# WARNING: This is experimental. Only use it if you can ensure that your application will properly handle thread affinity.
option(BABYLON_NATIVE_CHECK_THREAD_AFFINITY "Checks thread safety in the graphics device calls. It can be removed if hosting application ensures thread coherence." ON)

## Plugins
option(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE "Include Babylon Native Plugin ExternalTexture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAMERA "Include Babylon Native Plugin NativeCamera." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE "Include Babylon Native Plugin NativeCapture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE "Include Babylon Native Plugin NativeEngine." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEINPUT "Include Babylon Native Plugin NativeInput." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS "Include Babylon Native Plugin NativeOptimizations." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVETRACING "Include Babylon Native Plugin NativeTracing." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEXR "Include Babylon Native Plugin XR." ON)
option(BABYLON_NATIVE_PLUGIN_TESTUTILS "Include Babylon Native Plugin TestUtils." ON)

## Polyfills
option(BABYLON_NATIVE_POLYFILL_WINDOW "Include Babylon Native Polyfill Window." ON)
option(BABYLON_NATIVE_POLYFILL_CANVAS "Include Babylon Native Polyfill Canvas." ON)

# --------------------------------------------------

function(add_msvc_security_flags target)
    if(BABYLON_NATIVE_MSVC_SECURITY_FLAGS)
        # enable /Qspectre compiler flag in Visual Studio 2017 version 15.5.5 and later.
        if(MSVC_VERSION GREATER_EQUAL 1913)
            target_compile_options(${target} PRIVATE "/Qspectre")
        endif()
        # enable /ZH:SHA_256 compiler flag in Visual Studio 2019 version 16.4 and later.
        if(MSVC_VERSION GREATER_EQUAL 1924)
            target_compile_options(${target} PRIVATE "/ZH:SHA_256")
        endif()
        # The /guard:ehcont option is available in Visual Studio 2019 version 16.7 and later. The feature is supported for 64-bit processes on a 64-bit OS.
        if((MSVC_VERSION GREATER_EQUAL 1928) AND (CMAKE_SIZEOF_VOID_P EQUAL 8))
            target_compile_options(${target} PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/guard:ehcont>")
            target_link_options(${target} PRIVATE "$<$<NOT:$<CONFIG:DEBUG>>:/guard:ehcont>")
        endif()
    endif()
endfunction()

if(APPLE)
    # Without this option on azure pipelines, there is a mismatch with math.h giving a lot of undefined functions on macOS.
    # Only enabled for Apple as there is no issue for Windows.
    set(CMAKE_NO_SYSTEM_FROM_IMPORTED TRUE)
endif()

if(BABYLON_NATIVE_BUILD_APPS AND ((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    if(WIN32)
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        # Default build type for my test projects are /MDd (MultiThreaded DLL) but GTests default to /MTd (MultiTreaded)
        # see https://github.com/google/googletest/blob/main/googletest/README.md
        # "Enabling this option will make gtest link the runtimes dynamically too, and match the project in which it is included."
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    endif()
    set(CONTENT_TO_FETCH ${CONTENT_TO_FETCH} googletest)
endif()

# Setting Platform
if(ANDROID)
    set(BABYLON_NATIVE_PLATFORM "Android")
    set(CONTENT_TO_FETCH ${CONTENT_TO_FETCH} AndroidExtensions arcore-android-sdk)
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

if(WIN32 AND NOT ANDROID AND NOT IOS)
    # Avoid picking up system installed jsoncpp in favor of source distributed with openxr_loader project
    set(BUILD_WITH_SYSTEM_JSONCPP OFF BOOL "disable using system installed jsoncpp")
    set(CONTENT_TO_FETCH ${CONTENT_TO_FETCH} OpenXR-SDK OpenXR-MixedReality)
endif()

# Setting Graphics API
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

# Fetching content
message(STATUS "Fetching dependencies for ${PROJECT_NAME} (${CONTENT_TO_FETCH})")
FetchContent_MakeAvailable(${CONTENT_TO_FETCH})
message(STATUS "Fetching dependencies for ${PROJECT_NAME} - done")

if(BABYLON_NATIVE_BUILD_APPS AND ((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID)))
    set_property(TARGET gmock PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gmock_main PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest_main PROPERTY FOLDER Dependencies/GoogleTest)
endif()

if(APPLE)
    set(BABYLON_NATIVE_PLATFORM_IMPL_EXT "mm")
    set_property(TARGET JsRuntime PROPERTY UNITY_BUILD false)
    set_property(TARGET AppRuntime PROPERTY UNITY_BUILD false)
else()
    set(BABYLON_NATIVE_PLATFORM_IMPL_EXT "cpp")
endif()

if(UNIX AND NOT APPLE AND NOT ANDROID)
    # Use GLVND libraries for EGL support in bgfx
    set(OpenGL_GL_PREFERENCE GLVND)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    # __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
    # https://gitlab.kitware.com/cmake/cmake/-/issues/18837
    add_compile_options(/Zc:__cplusplus)

    # default to /W3
    add_compile_options(/W3)
endif()

if(WIN32)
    # UrlLib does not support unity build on win32
    set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)
endif()

add_subdirectory(Dependencies)
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Polyfills)

if(DEFINED EXTENSIONS_DIRS)
    foreach(extensionDir IN LISTS EXTENSIONS_DIRS)
        add_subdirectory(${extensionDir} "Extensions")
    endforeach()
endif()

if(BABYLON_NATIVE_BUILD_APPS)
    add_subdirectory(Apps)
endif()

if(BABYLON_NATIVE_INSTALL)
    include(Install/Install.cmake)
endif()

cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------

# EXCLUDE_FROM_ALL is used to prevent FetchContent from automatically adding
# targets to the ALL_BUILD target.  We do this for all dependencies except
# JsRuntimeHost because JsRuntimeHost has top-level targets (e.g., AppRuntime)
# that are part of the Babylon Native contract. When adding dependencies using
# FetchContent, add the EXCLUDE_FROM_ALL option except where there are top-level
# targets that are part of the Babylon Native contract.

FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
    GIT_TAG 2e85a8d43b89246c460112c9e5546ad54b6e87b4
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG fe41b17dedea47b5dead98f0271ca284e94ac6a8
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976
    EXCLUDE_FROM_ALL)
FetchContent_Declare(base-n
    GIT_REPOSITORY https://github.com/azawadzki/base-n.git
    GIT_TAG 7573e77c0b9b0e8a5fb63d96dbde212c921993b4
    EXCLUDE_FROM_ALL)
FetchContent_Declare(bgfx.cmake
    GIT_REPOSITORY https://github.com/BabylonJS/bgfx.cmake.git
    GIT_TAG 9c64597523a4edb3c54ef66f7ad1c7c3fa806ceb
    EXCLUDE_FROM_ALL)
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG 631780e42886e5f12bfd1a5568c7395f1d657f43
    EXCLUDE_FROM_ALL)
# Dawn (WebGPU) — only populated + built when BABYLON_NATIVE_PLUGIN_NATIVEDAWN is
# ON (see the guarded FetchContent_MakeAvailable below). Declaring it here is
# free; no clone happens unless the plugin is enabled. Pinned to a Dawn dated
# tag (a branch tip) so GIT_SHALLOW works: a raw commit SHA cannot be shallow-
# cloned because CMake's shallow path fetches only branch tips. This tag is the
# 2026-02-09 (Chrome 146) snapshot and contains commit
# a44d7a3d78f23c680491c0fc04f53a1df62e02ff (the reference pin) plus 7 commits.
# GIT_SUBMODULES "" disables submodule init: Dawn's .gitmodules declares ~53
# nested submodules (angle, swiftshader, catapult, dxc, ...), and FetchContent
# would otherwise recursively clone them all (very slow, multi-GB). They are not
# needed — DAWN_FETCH_DEPENDENCIES (below) runs Dawn's fetch script at configure
# time to pull only the deps the enabled backend requires.
FetchContent_Declare(dawn
    GIT_REPOSITORY https://github.com/google/dawn.git
    GIT_TAG        v20260209.194954
    GIT_SHALLOW    TRUE
    GIT_SUBMODULES ""
    EXCLUDE_FROM_ALL)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/BabylonJS/glslang.git
    GIT_TAG 284e4301e5a6b44b279635276588db7cdd942624
    EXCLUDE_FROM_ALL)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz"
    EXCLUDE_FROM_ALL)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.5.0
    EXCLUDE_FROM_ALL)
FetchContent_Declare(JsRuntimeHost
    GIT_REPOSITORY https://github.com/BabylonJS/JsRuntimeHost.git
    GIT_TAG 8cd142951018de07c89c23f726fb7dc9c8374599)
FetchContent_Declare(metal-cpp
    GIT_REPOSITORY https://github.com/bkaradzic/metal-cpp.git
    GIT_TAG metal-cpp_26
    EXCLUDE_FROM_ALL)
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/BabylonJS/SPIRV-Cross.git
    GIT_TAG a512817ddbcd879a3929aef7d1d762871bdf8635
    EXCLUDE_FROM_ALL)

# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

# BABYLON_NATIVE_BUILD_SOURCETREE flag is used by BabylonReactNative NPM build process.
# CMake script is run twice. At package creation, BABYLON_NATIVE_BUILD_SOURCETREE is ON
# which makes all dependencies to be fetched and packaged.
# At build time, cmake is run again on BabylonNative root but FETCHCONTENT_FULLY_DISCONNECTED is ON
# and FetchContent_MakeAvailable_With_Message will use provided local folder provided to cmake command line
# like '-DFETCHCONTENT_SOURCE_DIR_BGFX.CMAKE=../shared/Babylon/....'

if(VISIONOS OR IOS OR BABYLON_NATIVE_BUILD_SOURCETREE)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    if(NOT BABYLON_NATIVE_BUILD_SOURCETREE)
        set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    endif()
    set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
endif()

if(VISIONOS)
    set(PLATFORM "VISIONOSCOMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "1.0" CACHE STRING "")
elseif(IOS)
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "13" CACHE STRING "")
endif()

project(BabylonNative)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Suppress "Performing Test ..." output from check_c_compiler_flag() and similar macros.
set(CMAKE_REQUIRED_QUIET TRUE)

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
option(BABYLON_NATIVE_PLUGIN_NATIVEDAWN "Include experimental Babylon Native Plugin NativeDawn (WebGPU via Dawn)." OFF)
option(BABYLON_NATIVE_PLUGIN_NATIVEENCODING "Include Babylon Native Plugin NativeEncoding." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE "Include Babylon Native Plugin NativeEngine." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP "Enable WebP image parsing in NativeEngine (via bimg)." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES "Enable image loading in NativeEngine (LoadTexture, image bitmap, bimg)." ON)
if(NOT BABYLON_NATIVE_PLUGIN_NATIVEENGINE_LOAD_IMAGES)
    # WebP support relies on image loading code paths; force it off when image loading is disabled.
    set(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP OFF CACHE BOOL "Enable WebP image parsing in NativeEngine (via bimg)." FORCE)
endif()
option(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_COMPILESHADERS "Include Babylon Native Plugin NativeEngine - Compile Shaders." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEINPUT "Include Babylon Native Plugin NativeInput." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEOPTIMIZATIONS "Include Babylon Native Plugin NativeOptimizations." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVETRACING "Include Babylon Native Plugin NativeTracing." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVEXR "Include Babylon Native Plugin XR." ON)
option(BABYLON_NATIVE_PLUGIN_SHADERCACHE "Include Babylon Native Plugin ShaderCache." ON)
option(BABYLON_NATIVE_PLUGIN_SHADERCOMPILER "Include Babylon Native Plugin ShaderCompiler." ON)
option(BABYLON_NATIVE_PLUGIN_SHADERTOOL "Include Babylon Native Plugin ShaderTool." ON)
option(BABYLON_NATIVE_PLUGIN_TESTUTILS "Include Babylon Native Plugin TestUtils." ON)

# Polyfills
option(BABYLON_NATIVE_POLYFILL_ABORTCONTROLLER "Include Babylon Native Polyfill AbortController." ON)
option(BABYLON_NATIVE_POLYFILL_CANVAS "Include Babylon Native Polyfill Canvas." ON)
option(BABYLON_NATIVE_POLYFILL_SCHEDULING "Include Babylon Native Polyfill Scheduling." ON)
option(BABYLON_NATIVE_POLYFILL_URL "Include Babylon Native Polyfill URL." ON)
option(BABYLON_NATIVE_POLYFILL_WEBSOCKET "Include Babylon Native Polyfill WebSocket." ON)
option(BABYLON_NATIVE_POLYFILL_WINDOW "Include Babylon Native Polyfill Window." ON)

# NativeDawn (experimental WebGPU-via-Dawn backend) and NativeEngine (bgfx
# backend) are mutually exclusive: they are two implementations of the same
# rendering role. Enabling NativeDawn forces NativeEngine off so the Playground
# (and Embedding layer) install exactly one graphics backend / _native engine.
if(BABYLON_NATIVE_PLUGIN_NATIVEDAWN)
    if(BABYLON_NATIVE_PLUGIN_NATIVEENGINE)
        message(STATUS "BABYLON_NATIVE_PLUGIN_NATIVEDAWN is ON: forcing BABYLON_NATIVE_PLUGIN_NATIVEENGINE OFF (mutually exclusive backends).")
    endif()
    set(BABYLON_NATIVE_PLUGIN_NATIVEENGINE OFF CACHE BOOL "Include Babylon Native Plugin NativeEngine." FORCE)
endif()

# Embedding
option(BABYLON_NATIVE_EMBEDDING "Build the cross-platform Babylon::Embedding facade (Runtime + View)." ON)
option(BABYLON_NATIVE_EMBEDDING_ANDROID "Build the Android JNI interop layer for Babylon::Embedding." OFF)
# Default ON for Apple platforms — the iOS Playground (and future
# macOS / visionOS Playground migrations) consume it directly. Apple
# hosts that don't want the interop layer can explicitly disable.
option(BABYLON_NATIVE_EMBEDDING_APPLE "Build the Apple (iOS / macOS / visionOS) Obj-C++ interop layer for Babylon::Embedding." ${APPLE})

# Sanitizers
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

if(ENABLE_SANITIZERS)
    set(ENABLE_RTTI ON CACHE BOOL "" FORCE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZERS "address,undefined")
        # Check for Clang since vptr and fdsan are Clang-specific
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # vptr is not supported on Windows.
            if(NOT WIN32)
                list(APPEND SANITIZERS "vptr")
            endif()
            # FDSan only works on Android builds with Clang
            if (ANDROID)
                list(APPEND SANITIZERS "fdsan")
            endif()
        endif()

        string(JOIN "," SANITIZER_FLAGS ${SANITIZERS})

        # -fno-sanitize-recover=all makes UBSan findings fatal. Without it, UBSan
        # only prints a diagnostic to stderr and execution continues, so CI stays
        # green while latent UB goes unreported.
        add_compile_options(-fsanitize=${SANITIZER_FLAGS} -fno-omit-frame-pointer -fno-sanitize-recover=all)
        add_link_options(-fsanitize=${SANITIZER_FLAGS})
    elseif(MSVC)
        # MSVC only supports AddressSanitizer
        add_compile_options(/fsanitize=address /Zi /Od)
    else()
        message(FATAL_ERROR "Sanitizers not supported on this compiler.")
    endif()
endif()

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
    elseif(GRAPHICS_API STREQUAL "OpenGLWindowsDevOnly")
        # OpenGLES via ANGLE from a Chromium-based browser (dev-only).
        set(GRAPHICS_API OpenGL)
        set(BABYLON_NATIVE_OPENGLES_FROM_BROWSER ON)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan AND NOT GRAPHICS_API STREQUAL D3D11 AND NOT GRAPHICS_API STREQUAL D3D12)
            message(FATAL_ERROR "Unrecognized/Unsupported render API: ${GRAPHICS_API}")
        endif()
    endif()
endif()

# Find ANGLE libraries from Edge or Chrome when using OpenGLWindowsDevOnly.
if(BABYLON_NATIVE_OPENGLES_FROM_BROWSER)
    foreach(_browser_exe "msedge.exe" "chrome.exe")
        if(ANGLE_LIBEGL)
            break()
        endif()
        foreach(_reg_root "HKLM" "HKCU")
            if(ANGLE_LIBEGL)
                break()
            endif()
            execute_process(
                COMMAND reg query "${_reg_root}\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Paths\\${_browser_exe}" /ve
                OUTPUT_VARIABLE _reg_output
                ERROR_QUIET
                RESULT_VARIABLE _reg_result
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
            if(NOT _reg_result EQUAL 0)
                continue()
            endif()
            string(REGEX MATCH "REG_SZ[ \t]+([^\r\n]+)" _ "${_reg_output}")
            string(STRIP "${CMAKE_MATCH_1}" _browser_path)
            if(NOT _browser_path)
                continue()
            endif()
            cmake_path(GET _browser_path PARENT_PATH _browser_dir)
            file(GLOB _version_dirs LIST_DIRECTORIES true "${_browser_dir}/[0-9]*")
            list(SORT _version_dirs ORDER DESCENDING)
            foreach(_version_dir ${_version_dirs})
                if(IS_DIRECTORY "${_version_dir}" AND EXISTS "${_version_dir}/libEGL.dll")
                    set(ANGLE_LIBEGL "${_version_dir}/libEGL.dll")
                    set(ANGLE_LIBGLESV2 "${_version_dir}/libGLESv2.dll")
                    message(STATUS "Found ANGLE libraries in: ${_version_dir}")
                    break()
                endif()
            endforeach()
        endforeach()
    endforeach()

    if(NOT ANGLE_LIBEGL)
        message(FATAL_ERROR "OpenGLWindowsDevOnly requires ANGLE libraries (libEGL.dll, libGLESv2.dll) from Edge or Chrome, but none were found.")
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

    # Enable multiprocessor compilation for faster builds
    add_compile_options(/MP)
endif()

if(APPLE)
    # Create scheme for installation and other targets
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
endif()

if(BABYLON_DEBUG_TRACE)
    add_compile_definitions(BABYLON_DEBUG_TRACE)
endif()

add_subdirectory(Dependencies)

# ---- Experimental NativeDawn plugin: pull in Dawn (WebGPU) ----
# Dawn is a large dependency, so it is only fetched + built when NativeDawn is
# enabled. It is declared (git) alongside the other dependencies above but only
# populated here, inside this guard, so builds without
# BABYLON_NATIVE_PLUGIN_NATIVEDAWN never clone Dawn.
if(BABYLON_NATIVE_PLUGIN_NATIVEDAWN)
    set(DAWN_FETCH_DEPENDENCIES ON CACHE BOOL "" FORCE)
    set(DAWN_BUILD_SAMPLES   OFF CACHE BOOL "" FORCE)
    set(DAWN_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
    # No windowing/surface toolkit is needed: the NativeDawn plugin creates its
    # own surface (Win32-only) and builds surfaceless elsewhere. Disabling GLFW
    # (and X11/Wayland) avoids Dawn configuring GLFW, which otherwise requires
    # X11 dev packages (libxrandr-dev etc.) on Linux CI runners.
    set(DAWN_USE_GLFW    OFF CACHE BOOL "" FORCE)
    set(DAWN_USE_X11     OFF CACHE BOOL "" FORCE)
    set(DAWN_USE_WAYLAND OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_TESTS     OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_CMD_TOOLS OFF CACHE BOOL "" FORCE)

    # Build exactly one GPU backend per platform, with the matching Tint shader
    # writer. The WGSL reader (input) is always required; the SPIR-V/GLSL readers,
    # the WGSL writer and the IR-binary/GLSL-validator are never needed here.
    set(TINT_BUILD_SPV_READER     OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_WGSL_READER    ON  CACHE BOOL "" FORCE)
    set(TINT_BUILD_WGSL_WRITER    OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_GLSL_WRITER    OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_GLSL_VALIDATOR OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_IR_BINARY      OFF CACHE BOOL "" FORCE)

    # Start with every GPU backend + shader writer off; enable one set per platform.
    set(DAWN_ENABLE_D3D11    OFF CACHE BOOL "" FORCE)
    set(DAWN_ENABLE_D3D12    OFF CACHE BOOL "" FORCE)
    set(DAWN_ENABLE_VULKAN   OFF CACHE BOOL "" FORCE)
    set(DAWN_ENABLE_OPENGLES OFF CACHE BOOL "" FORCE)
    set(DAWN_ENABLE_METAL    OFF CACHE BOOL "" FORCE)
    set(DAWN_ENABLE_NULL     OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_HLSL_WRITER OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_MSL_WRITER  OFF CACHE BOOL "" FORCE)
    set(TINT_BUILD_SPV_WRITER  OFF CACHE BOOL "" FORCE)

    if(WIN32)
        # Windows: Direct3D 12 (HLSL).
        set(DAWN_ENABLE_D3D12      ON CACHE BOOL "" FORCE)
        set(TINT_BUILD_HLSL_WRITER ON CACHE BOOL "" FORCE)
    elseif(APPLE)
        # macOS / iOS: Metal (MSL).
        set(DAWN_ENABLE_METAL      ON CACHE BOOL "" FORCE)
        set(TINT_BUILD_MSL_WRITER  ON CACHE BOOL "" FORCE)
    else()
        # Android / Linux: Vulkan (SPIR-V).
        set(DAWN_ENABLE_VULKAN     ON CACHE BOOL "" FORCE)
        set(TINT_BUILD_SPV_WRITER  ON CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable_With_Message(dawn)
endif()
add_subdirectory(Core)
add_subdirectory(Plugins)
add_subdirectory(Polyfills)

if(BABYLON_NATIVE_EMBEDDING)
    add_subdirectory(Embedding)
endif()

if(BABYLON_NATIVE_BUILD_APPS)
    add_subdirectory(Apps)
endif()

# no install for glslang compiled for shaderc
if(BABYLON_NATIVE_INSTALL)
    include(Install/Install.cmake)
endif()

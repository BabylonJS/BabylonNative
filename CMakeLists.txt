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
    GIT_TAG 2d5af72259cc73e5f249d3c99bee2010be9cb042
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG c02527c32d51b6b3ffeda36f8cf140d2e1c60bb9
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976
    EXCLUDE_FROM_ALL)
FetchContent_Declare(base-n
    GIT_REPOSITORY https://github.com/azawadzki/base-n.git
    GIT_TAG 7573e77c0b9b0e8a5fb63d96dbde212c921993b4
    EXCLUDE_FROM_ALL)
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419
    EXCLUDE_FROM_ALL)
FetchContent_Declare(glslang
    GIT_REPOSITORY https://github.com/BabylonJS/glslang.git
    GIT_TAG 39a80699a315cb7f66c4ab3180edd4e2910fab28
    EXCLUDE_FROM_ALL)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.5.0
    EXCLUDE_FROM_ALL)
FetchContent_Declare(JsRuntimeHost
    GIT_REPOSITORY https://github.com/BabylonJS/JsRuntimeHost.git
    GIT_TAG 06d4420e92f9555b5356071444cd13767d3e4eb8)
FetchContent_Declare(SPIRV-Cross
    GIT_REPOSITORY https://github.com/BabylonJS/SPIRV-Cross.git
    GIT_TAG 6277baf62877f6b1a66a5801da94f0c9276991dc
    EXCLUDE_FROM_ALL)
FetchContent_Declare(libwebp
    GIT_REPOSITORY https://github.com/webmproject/libwebp.git
    GIT_TAG 57e324e2eb99be46df46d77b65705e34a7ae616c
    EXCLUDE_FROM_ALL)
FetchContent_Declare(webgpu_headers
    GIT_REPOSITORY https://github.com/webgpu-native/webgpu-headers.git
    GIT_TAG bac520839ff5ed2e2b648ed540bd9ec45edbccbc
    EXCLUDE_FROM_ALL)
FetchContent_Declare(wgpu_native
    GIT_REPOSITORY https://github.com/gfx-rs/wgpu-native.git
    GIT_TAG ba4deb5d935652f40c7e051b15cbb5d097219941
    EXCLUDE_FROM_ALL)

# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

# BABYLON_NATIVE_BUILD_SOURCETREE flag is used by BabylonReactNative NPM build process.
# CMake script is run twice. At package creation, BABYLON_NATIVE_BUILD_SOURCETREE is ON
# which makes all dependencies to be fetched and packaged.
# At build time, cmake is run again on BabylonNative root but FETCHCONTENT_FULLY_DISCONNECTED is ON
# and FetchContent_MakeAvailable_With_Message will use provided local folder provided to cmake command line
# like '-DFETCHCONTENT_SOURCE_DIR_JSRUNTIMEHOST=../shared/Babylon/....'

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

# Keep deployment targeting consistent across all generated Xcode targets.
# Without this, transitive static libraries may default to the host SDK version
# and become unloadable on older simulator runtimes.
if(IOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "${DEPLOYMENT_TARGET}" CACHE STRING "" FORCE)
    set(CMAKE_XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "${DEPLOYMENT_TARGET}")
elseif(VISIONOS)
    set(CMAKE_OSX_DEPLOYMENT_TARGET "${DEPLOYMENT_TARGET}" CACHE STRING "" FORCE)
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
option(BABYLON_NATIVE_PLUGIN_NATIVEINPUT "Include Babylon Native Plugin NativeInput." ON)
# NativeWebGPU/Canvas options are hardwired ON for the wgpu branch (see FORCE overrides below).
# TODO(bgfx-removal): Convert these back to user-facing option() toggles once the bgfx backend
# is fully removed and the wgpu path is the only backend.
option(BABYLON_NATIVE_BUILD_WEBGPU_TESTS "Build NativeWebGPU async bridge gtests on desktop hosts." OFF)
option(BABYLON_NATIVE_ENABLE_WEBGPU_DEVELOPER_FEATURES "Expose non-standard WebGPU developer hooks (Chromium/WebKit developer-features analog)." ON)
option(BABYLON_NATIVE_ENABLE_UNSAFE_WEBGPU "Expose non-standard unsafe WebGPU hooks (Chromium enable-unsafe-webgpu analog)." ON)

# Polyfills
option(BABYLON_NATIVE_POLYFILL_WINDOW "Include Babylon Native Polyfill Window." ON)

# Sanitizers
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

if(ENABLE_SANITIZERS)
    include(CheckCCompilerFlag)
    include(CheckCXXCompilerFlag)
    set(ENABLE_RTTI ON CACHE BOOL "" FORCE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        # Android app processes require explicit sanitizer runtime packaging.
        # Prefer ASan there for actionable heap safety checks with manageable
        # signal-to-noise in emulator/device runs.
        if(ANDROID)
            set(SANITIZERS "address")
        else()
            set(SANITIZERS "address,undefined")
        endif()
        # Check for Clang since vptr and fdsan are Clang-specific
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            # vptr depends on UBSan runtime and is useful on non-Android hosts.
            if(NOT ANDROID)
                list(APPEND SANITIZERS "vptr")
            endif()
            # FDSan only works on Android builds with Clang
            if (ANDROID)
                check_c_compiler_flag("-fsanitize=fdsan" BABYLON_NATIVE_HAS_C_FDSAN)
                check_cxx_compiler_flag("-fsanitize=fdsan" BABYLON_NATIVE_HAS_CXX_FDSAN)
                if(BABYLON_NATIVE_HAS_C_FDSAN AND BABYLON_NATIVE_HAS_CXX_FDSAN)
                    list(APPEND SANITIZERS "fdsan")
                else()
                    message(STATUS "Skipping fdsan sanitizer: compiler does not support -fsanitize=fdsan")
                endif()
            endif()
        endif()

        string(JOIN "," SANITIZER_FLAGS ${SANITIZERS})
        # Export for wrapper launch targets (e.g. Android JNI host) that are
        # created outside this directory scope.
        set(BABYLON_NATIVE_SANITIZER_FLAGS "${SANITIZER_FLAGS}" CACHE INTERNAL "Babylon Native sanitizer flag list" FORCE)

        add_compile_options(-fsanitize=${SANITIZER_FLAGS} -fno-omit-frame-pointer)
        add_link_options(-fsanitize=${SANITIZER_FLAGS})
    elseif(MSVC)
        # MSVC only supports AddressSanitizer
        add_compile_options(/fsanitize=address /Zi /Od)
    else()
        message(FATAL_ERROR "Sanitizers not supported on this compiler.")
    endif()
endif()

# Hardwired for wgpu branch: these are not user-configurable until bgfx is fully removed.
# TODO(bgfx-removal): Remove FORCE overrides and restore as user-facing option() toggles.
set(BABYLON_NATIVE_INSTALL OFF CACHE BOOL "Include the install target." FORCE)
set(BABYLON_NATIVE_PLUGIN_NATIVEINPUT ON CACHE BOOL "Include Babylon Native Plugin NativeInput." FORCE)
set(BABYLON_NATIVE_PLUGIN_NATIVEWEBGPU ON CACHE BOOL "Include Babylon Native Plugin NativeWebGPU." FORCE)
set(BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE ON CACHE BOOL "Use upstream wgpu-native as the Rust backend base." FORCE)
set(BABYLON_NATIVE_POLYFILL_WINDOW ON CACHE BOOL "Include Babylon Native Polyfill Window." FORCE)
set(BABYLON_NATIVE_POLYFILL_CANVAS ON CACHE BOOL "Include Babylon Native Polyfill Canvas." FORCE)
set(BABYLON_NATIVE_POLYFILL_CANVAS_WGPU ON CACHE BOOL "Use the wgpu/femtovg Canvas backend when Canvas polyfill is enabled." FORCE)

if(BABYLON_NATIVE_WGPU_USE_UPSTREAM_NATIVE)
    FetchContent_MakeAvailable_With_Message(webgpu_headers)
    FetchContent_MakeAvailable_With_Message(wgpu_native)
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
        set(GRAPHICS_API Vulkan)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan)
            message(FATAL_ERROR "Unrecognized/Unsupported render API: ${GRAPHICS_API}")
        endif()
    endif()
elseif(WIN32)
    if(NOT GRAPHICS_API)
        set(GRAPHICS_API D3D12)
    else()
        if(NOT GRAPHICS_API STREQUAL Vulkan AND NOT GRAPHICS_API STREQUAL D3D12)
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
    set(ENABLE_ARC ON CACHE STRING "Enables or disables ARC support." FORCE)
    if(NOT CMAKE_GENERATOR MATCHES "Xcode")
        # Some fetched dependencies gate ARC behind Xcode-only attributes.
        # Apply ARC explicitly for non-Xcode generators (Makefiles/Ninja).
        add_compile_options(-fobjc-arc)
    endif()

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
if(BABYLON_NATIVE_INSTALL)
    include(Install/Install.cmake)
endif()

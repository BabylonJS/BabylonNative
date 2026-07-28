cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)
include(${CMAKE_CURRENT_SOURCE_DIR}/Scripts/extensions.cmake)

# --------------------------------------------------
# Declarations
# --------------------------------------------------

# EXCLUDE_FROM_ALL is used to prevent FetchContent from automatically adding
# targets to the ALL_BUILD target. When adding dependencies using FetchContent,
# add the EXCLUDE_FROM_ALL option except where there are top-level targets that
# are part of the Babylon Native contract.
#
# Most dependencies are now vendored directly in the monorepo (see Dependencies/,
# and Core/ + Polyfills/ for what used to be the JsRuntimeHost repository), so
# only the remaining externally fetched ones are declared here.

FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG fe41b17dedea47b5dead98f0271ca284e94ac6a8
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcore-android-sdk
    GIT_REPOSITORY https://github.com/google-ar/arcore-android-sdk.git
    GIT_TAG 98cb803de5482fb2b36effe8be3b5a0d3b726976
    EXCLUDE_FROM_ALL)
FetchContent_Declare(asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG f693a3eb7fe72a5f19b975289afc4f437d373d9c
    EXCLUDE_FROM_ALL)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz"
    EXCLUDE_FROM_ALL)
FetchContent_Declare(hermes
    GIT_REPOSITORY https://github.com/facebook/hermes.git
    GIT_TAG 348582831f50954895da8e80cc91112d51036c69
    EXCLUDE_FROM_ALL)
FetchContent_Declare(llhttp
    URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz"
    EXCLUDE_FROM_ALL)
FetchContent_Declare(quickjs-ng
    GIT_REPOSITORY https://github.com/quickjs-ng/quickjs.git
    GIT_TAG 93d3f7df465027f487ed37e175a0bc3012fee79e
    EXCLUDE_FROM_ALL)

# --------------------------------------------------
# BABYLON_NATIVE_BUILD_SOURCETREE flag is used by BabylonReactNative NPM build process.
# CMake script is run twice. At package creation, BABYLON_NATIVE_BUILD_SOURCETREE is ON
# which makes all dependencies to be fetched and packaged.
# At build time, cmake is run again on BabylonNative root but FETCHCONTENT_FULLY_DISCONNECTED is ON
# and FetchContent_MakeAvailable_With_Message will use provided local folder provided to cmake command line
# like '-DFETCHCONTENT_SOURCE_DIR_BGFX.CMAKE=../shared/Babylon/....'

if(VISIONOS OR IOS OR BABYLON_NATIVE_BUILD_SOURCETREE)
    set(ios-cmake_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/ios-cmake")
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

# Apps/UnitTests is built everywhere. Its JsRuntimeHost half is pure JavaScript
# runtime coverage and needs nothing but a JS engine, so it runs on every
# platform. Its Babylon Native half drives a real graphics device through a
# platform window, which only exists on the desktop hosts, so those tests are
# opt-out there and unavailable elsewhere.
if((WIN32 AND NOT WINDOWS_STORE) OR (APPLE AND NOT IOS AND NOT VISIONOS) OR (UNIX AND NOT ANDROID AND NOT APPLE))
    set(BABYLON_NATIVE_UNIT_TESTS_GRAPHICS_DEFAULT ON)
else()
    set(BABYLON_NATIVE_UNIT_TESTS_GRAPHICS_DEFAULT OFF)
endif()
option(BABYLON_NATIVE_UNIT_TESTS_GRAPHICS "Include the Babylon Native graphics tests in the UnitTests app." ${BABYLON_NATIVE_UNIT_TESTS_GRAPHICS_DEFAULT})

# WARNING: This is experimental. Only use it if you can ensure that your application will properly handle thread affinity.
option(BABYLON_NATIVE_CHECK_THREAD_AFFINITY "Checks thread safety in the graphics device calls. It can be removed if hosting application ensures thread coherence." ON)

# Plugins
option(BABYLON_NATIVE_PLUGIN_EXTERNALTEXTURE "Include Babylon Native Plugin ExternalTexture." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAMERA "Include Babylon Native Plugin NativeCamera." ON)
option(BABYLON_NATIVE_PLUGIN_NATIVECAPTURE "Include Babylon Native Plugin NativeCapture." ON)
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

# JsRuntimeHost (vendored under Core/ and Polyfills/ since the monorepo merge)
option(JSRUNTIMEHOST_TESTS "Include JsRuntimeHost Tests." OFF)
option(NAPI_BUILD_ABI "Build the ABI layer." ON)

# JsRuntimeHost - Core
option(JSRUNTIMEHOST_CORE_APPRUNTIME "Include JsRuntimeHost Core AppRuntime" ON)
option(JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR "Include the V8 inspector protocol required to debug JavaScript with a V8 debugger." ON)
option(JSRUNTIMEHOST_CORE_SCRIPTLOADER "Include JsRuntimeHost Core ScriptLoader" ON)

# JsRuntimeHost - Polyfills
option(JSRUNTIMEHOST_POLYFILL_CONSOLE "Include JsRuntimeHost Polyfill Console." ON)
option(JSRUNTIMEHOST_POLYFILL_SCHEDULING "Include JsRuntimeHost Polyfill Scheduling." ON)
option(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST "Include JsRuntimeHost Polyfill XMLHttpRequest." ON)
option(JSRUNTIMEHOST_POLYFILL_FETCH "Include JsRuntimeHost Polyfill fetch." ON)
option(JSRUNTIMEHOST_POLYFILL_URL "Include JsRuntimeHost Polyfill URL and URLSearchParams." ON)
option(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER "Include JsRuntimeHost Polyfills AbortController and AbortSignal." ON)
option(JSRUNTIMEHOST_POLYFILL_WEBSOCKET "Include JsRuntimeHost Polyfill WebSocket." ON)
option(JSRUNTIMEHOST_POLYFILL_BLOB "Include JsRuntimeHost Polyfill Blob." ON)
option(JSRUNTIMEHOST_POLYFILL_FILE "Include JsRuntimeHost Polyfill File and FileReader." ON)
option(JSRUNTIMEHOST_POLYFILL_PERFORMANCE "Include JsRuntimeHost Polyfill Performance." ON)
option(JSRUNTIMEHOST_POLYFILL_TEXTDECODER "Include JsRuntimeHost Polyfill TextDecoder." ON)
option(JSRUNTIMEHOST_POLYFILL_TEXTENCODER "Include JsRuntimeHost Polyfill TextEncoder." ON)

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

# JsRuntimeHost uses its own platform naming for its per-platform source files
# (Core/AppRuntime/Source/AppRuntime_<platform>.*).
if(ANDROID)
    set(JSRUNTIMEHOST_PLATFORM "Android")
elseif(IOS OR VISIONOS)
    set(JSRUNTIMEHOST_PLATFORM "iOS")
elseif(APPLE)
    set(JSRUNTIMEHOST_PLATFORM "macOS")
elseif(WINDOWS_STORE)
    set(JSRUNTIMEHOST_PLATFORM "UWP")
elseif(WIN32)
    set(JSRUNTIMEHOST_PLATFORM "Win32")
elseif(UNIX)
    set(JSRUNTIMEHOST_PLATFORM "Unix")
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

# Select the Node-API JavaScript engine before descending into Dependencies:
# the engine determines which engine-specific dependencies (QuickJS, Hermes,
# asio/llhttp for the V8 inspector) have to be made available there. The same
# per-platform defaults are repeated in Core/Node-API/CMakeLists.txt so that
# directory still works standalone; `set(... CACHE ...)` makes that a no-op.
if(WIN32)
    set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for Node-API")
elseif(APPLE)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
elseif(ANDROID)
    set(NAPI_JAVASCRIPT_ENGINE "V8" CACHE STRING "JavaScript engine for Node-API")
elseif(UNIX)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
endif()

# Apps/UnitTests drives arcana's blocking queue deterministically to reproduce
# shutdown races (see AppRuntime.DestroyDoesNotDeadlock). The hook lives inside
# arcana's header-only queue, so it has to be enabled for *every* translation
# unit that instantiates one -- notably Core/AppRuntime -- not just the test
# target, otherwise the test observes a queue that never calls the hook.
if(BABYLON_NATIVE_BUILD_APPS OR JSRUNTIMEHOST_TESTS)
    add_compile_definitions(ARCANA_TEST_HOOKS)
endif()

add_subdirectory(Dependencies)
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

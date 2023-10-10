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
    GIT_TAG 04d91f6675dabb3c97df346a32f6184b0a7ef845)
FetchContent_Declare(arcana
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG f2757396e80bc4169f2ddb938ce25367a98ffdd0)
FetchContent_Declare(JsRuntimeHost
    GIT_REPOSITORY https://github.com/BabylonJS/JsRuntimeHost.git
    GIT_TAG 3f7714335799554b54c64f035db8dc26d3dcf517)
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
    GIT_TAG 1a47db416ec2aae3f51b28b94f73e8f54e412d0d)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz")

set(CONTENT_TO_FETCH cmake-extensions arcana JsRuntimeHost)

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
option(BABYLON_NATIVE_ASYNC_SHADER_COMPILATION "Enables async shader compilation in Babylon Native" ON)

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
    set(CONTENT_TO_FETCH ${CONTENT_TO_FETCH} AndroidExtensions)
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

# __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
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

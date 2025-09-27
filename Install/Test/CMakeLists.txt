cmake_minimum_required(VERSION 3.18)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz")
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

project(TestInstall)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ANDROID OR CMAKE_SYSTEM_NAME STREQUAL "Android")
    add_compile_definitions(_LIBCPP_ENABLE_CXX20_REMOVED_CHAR_TRAITS_SPECIALIZATIONS)
    string(APPEND CMAKE_CXX_FLAGS " -D_LIBCPP_ENABLE_CXX20_REMOVED_CHAR_TRAITS_SPECIALIZATIONS")

    set(_babylon_android_char_traits_header "${CMAKE_SOURCE_DIR}/Dependencies/Android/std_char_traits_uint16_t.h")
    if(EXISTS "${_babylon_android_char_traits_header}")
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:-include${_babylon_android_char_traits_header}>)
    endif()
endif()

if(WIN32 OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID AND NOT APPLE))
    FetchContent_MakeAvailable_With_Message(googletest)
endif()

# __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

set(USE_SYSTEM_V8 FALSE)
if(ANDROID)
    set(_V8_INCLUDE_HINTS)
    if(DEFINED ENV{V8_INCLUDE_DIR})
        list(APPEND _V8_INCLUDE_HINTS $ENV{V8_INCLUDE_DIR})
    endif()
    if(DEFINED V8_INCLUDE_DIR)
        list(APPEND _V8_INCLUDE_HINTS ${V8_INCLUDE_DIR})
    endif()

    find_path(SYSTEM_V8_INCLUDE_DIR
        NAMES v8.h
        HINTS ${_V8_INCLUDE_HINTS})

    if(SYSTEM_V8_INCLUDE_DIR)
        set(_V8_LIB_HINTS)
        if(DEFINED ENV{V8_LIB_DIR})
            list(APPEND _V8_LIB_HINTS $ENV{V8_LIB_DIR})
        endif()
        if(DEFINED V8_LIB_DIR)
            list(APPEND _V8_LIB_HINTS ${V8_LIB_DIR})
        endif()

        find_library(SYSTEM_V8_MONOLITH_LIB NAMES v8_monolith v8 HINTS ${_V8_LIB_HINTS})
        find_library(SYSTEM_V8_LIBBASE_LIB NAMES v8_libbase HINTS ${_V8_LIB_HINTS})
        find_library(SYSTEM_V8_LIBPLATFORM_LIB NAMES v8_libplatform HINTS ${_V8_LIB_HINTS})
        find_library(SYSTEM_V8_ICUUC_LIB NAMES icuuc HINTS ${_V8_LIB_HINTS})
        find_library(SYSTEM_V8_ICUI18N_LIB NAMES icui18n HINTS ${_V8_LIB_HINTS})
        find_library(SYSTEM_V8_ZLIB_LIB NAMES z zlib HINTS ${_V8_LIB_HINTS})

        if(SYSTEM_V8_MONOLITH_LIB)
            set(USE_SYSTEM_V8 TRUE)
        endif()
    endif()
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "")
    if(APPLE)
        message(STATUS "No JS Engine provided. Defaulting to JavaScriptCore on Apple platforms.")
        set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore")
    elseif(ANDROID AND USE_SYSTEM_V8)
        message(STATUS "No JS Engine provided. Defaulting to system-installed V8 for Android builds.")
        set(NAPI_JAVASCRIPT_ENGINE "V8")
    else()
        message("No JS Engine provided. Defaulting to Chakra.")
        set(NAPI_JAVASCRIPT_ENGINE "Chakra")
    endif()
endif()

# executable stub

set(APPS_DIR "../../Apps")
set(UNITTESTS_DIR "${APPS_DIR}/UnitTests")
set(LOCAL_SCRIPTS "${UNITTESTS_DIR}/Scripts/tests.js")

set(NPM_SCRIPTS
    "${APPS_DIR}/node_modules/babylonjs/babylon.max.js"
    "${APPS_DIR}/node_modules/babylonjs-materials/babylonjs.materials.js"
    "${APPS_DIR}/node_modules/chai/chai.js"
    "${APPS_DIR}/node_modules/mocha/mocha.js")

set(SOURCES
    "${UNITTESTS_DIR}/Shared/Shared.h"
    "${UNITTESTS_DIR}/Shared/Shared.cpp")

if(WIN32 AND NOT WINDOWS_STORE)
    set(SOURCES ${SOURCES} "${UNITTESTS_DIR}/Win32/App.cpp")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(NUGET_PATH "${BINARY_DIR}/NuGet")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(V8JSI_VERSION "0.64.33")
    set(CPU_ARCH "x64")
    set(PLATFORM_FOLDER "win32")
    set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
    set(V8JSI_LIB_PATH_RELEASE "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Release/${CPU_ARCH}/")
    set(SOURCES ${SOURCES} "${V8JSI_PACKAGE_PATH}/build/native/jsi/jsi/jsi.cpp")
    set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} "v8jsi.dll.lib")
elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    if(USE_SYSTEM_V8 AND NOT WIN32)
        message(STATUS "Linking against system-installed V8 libraries from ${SYSTEM_V8_MONOLITH_LIB}")
        set(V8_INCLUDE_DIR ${SYSTEM_V8_INCLUDE_DIR})

        set(_SYSTEM_V8_LIBS ${SYSTEM_V8_MONOLITH_LIB})
        if(SYSTEM_V8_LIBBASE_LIB)
            list(APPEND _SYSTEM_V8_LIBS ${SYSTEM_V8_LIBBASE_LIB})
        endif()
        if(SYSTEM_V8_LIBPLATFORM_LIB)
            list(APPEND _SYSTEM_V8_LIBS ${SYSTEM_V8_LIBPLATFORM_LIB})
        endif()
        if(SYSTEM_V8_ICUUC_LIB)
            list(APPEND _SYSTEM_V8_LIBS ${SYSTEM_V8_ICUUC_LIB})
        endif()
        if(SYSTEM_V8_ICUI18N_LIB)
            list(APPEND _SYSTEM_V8_LIBS ${SYSTEM_V8_ICUI18N_LIB})
        endif()
        if(SYSTEM_V8_ZLIB_LIB)
            list(APPEND _SYSTEM_V8_LIBS ${SYSTEM_V8_ZLIB_LIB})
        endif()

        find_package(Threads REQUIRED)
        if(CMAKE_THREAD_LIBS_INIT)
            list(APPEND _SYSTEM_V8_LIBS ${CMAKE_THREAD_LIBS_INIT})
        endif()
        if(UNIX AND NOT APPLE)
            list(APPEND _SYSTEM_V8_LIBS dl)
        endif()

        set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} ${_SYSTEM_V8_LIBS})
    else()
        set_cpu_platform_arch()
        set(V8_VERSION "11.9.169.4")
        download_nuget()
        set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v143-${CPU_ARCH}.${V8_VERSION}")
        set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v143-${CPU_ARCH}.${V8_VERSION}")
        set(V8_REDIST_PACKAGE_PATH "${NUGET_PATH}/packages/v8.redist-v143-${CPU_ARCH}.${V8_VERSION}")

        add_library(v8_libbase SHARED IMPORTED)
        set_target_properties(v8_libbase PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libbase.dll.lib")
        add_library(v8_libplatform SHARED IMPORTED)
        set_target_properties(v8_libplatform PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libplatform.dll.lib")
        add_library(v8 SHARED IMPORTED)
        set_target_properties(v8 PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8.dll.lib")
        target_link_libraries(v8 INTERFACE v8_libbase INTERFACE v8_libplatform)
        target_include_directories(v8 INTERFACE "${V8_PACKAGE_PATH}/include")

        set(V8_DIST
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/icudtl.dat"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/third_party_icu_icui18n.dll"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/third_party_abseil-cpp_absl.dll"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/icuuc.dll"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8.dll"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libbase.dll"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libplatform.dll"
            "${V8_REDIST_PACKAGE_PATH}/lib/Release/third_party_zlib.dll")

        # only 1 imported location per library -> Adding 1 library per file
        foreach(V8FILE ${V8_DIST})
            get_filename_component(V8FILE_NAME "${V8FILE}" NAME_WE)
            add_library("v8::${V8FILE_NAME}" SHARED IMPORTED)
            set_target_properties("v8::${V8FILE_NAME}" PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libbase.dll.lib")
            set_target_properties("v8::${V8FILE_NAME}" PROPERTIES IMPORTED_LOCATION ${V8FILE})
        endforeach()

        set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES}
            v8
            v8inspector
            llhttp
            v8::icudtl
            v8::third_party_icu_icui18n
            v8::icuuc
            v8::v8
            v8::v8_libbase
            v8::v8_libplatform
            v8::third_party_zlib)

        if(CPU_ARCH STREQUAL "x64")
            # Enable V8 Pointer Compression
            # https://v8.dev/blog/pointer-compression
            # https://stackoverflow.com/q/62921373
            set(NAPI_DEFINITIONS PUBLIC V8_COMPRESS_POINTERS)
        endif()
    endif()
endif()

add_executable(TestInstall ${LOCAL_SCRIPTS} ${NPM_SCRIPTS} ${SOURCES})

set(INSTALL_LIBS_DIR "${BINARY_DIR}/install/lib")
target_include_directories(TestInstall PRIVATE "${BINARY_DIR}/install/include")
if(V8_INCLUDE_DIR)
    target_include_directories(TestInstall PRIVATE "${V8_INCLUDE_DIR}")
endif()
target_link_directories(TestInstall PRIVATE ${INSTALL_LIBS_DIR} ${V8JSI_LIB_PATH_RELEASE})

if(ANDROID OR IOS)
    set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} NativeXr)
endif()

target_link_libraries(TestInstall
    AppRuntime
    arcana
    bimg_encode
    bimg_decode
    minz
    bgfx
    bimg
    bx
    Canvas
    Console
    ExternalTexture
    Foundation
    GenericCodeGen
    glslang
    glslang-default-resource-limits
    Graphics
    gtest_main
    JsRuntime
    libsharpyuv.lib
    libwebp.lib
    libwebpdecoder.lib
    libwebpdemux.lib
    MachineIndependent
    napi
    NativeCamera
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    OGLCompiler
    OSDependent
    ScriptLoader
    Scheduling
    spirv-cross-core
    spirv-cross-hlsl
    SPIRV
    UrlLib
    Window
    XMLHttpRequest
    chakrart
    d3d11
    d3d12
    d3dcompiler
    Pathcch
    runtimeobject.lib # for WINRT_IMPL_RoXXX functions
    ${ADDITIONAL_LIBRARIES}
)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET TestInstall POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:TestInstall>>,copy,true> $<TARGET_RUNTIME_DLLS:TestInstall> $<TARGET_FILE_DIR:TestInstall> COMMAND_EXPAND_LISTS)

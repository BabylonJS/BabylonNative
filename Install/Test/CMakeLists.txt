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

if(WIN32 OR (APPLE AND NOT IOS) OR (UNIX AND NOT ANDROID AND NOT APPLE))
    FetchContent_MakeAvailable_With_Message(googletest)
endif()

# __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837
if(MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "")
    message("No JS Engine provided. Defaulting to Chakra.")
    set(NAPI_JAVASCRIPT_ENGINE "Chakra")
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

add_executable(TestInstall ${LOCAL_SCRIPTS} ${NPM_SCRIPTS} ${SOURCES})

set(INSTALL_LIBS_DIR "${BINARY_DIR}/install/lib")
target_include_directories(TestInstall PRIVATE "${BINARY_DIR}/install/include")
target_link_directories(TestInstall PRIVATE ${INSTALL_LIBS_DIR} ${V8JSI_LIB_PATH_RELEASE})

if(ANDROID OR IOS)
    set(ADDITIONAL_LIBRARIES ${ADDITIONAL_LIBRARIES} NativeXr)
endif()

target_link_libraries(TestInstall
    AppRuntime
    arcana
    bimg_encode
    bimg_decode
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
    ${ADDITIONAL_LIBRARIES}
)

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET TestInstall POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:TestInstall>>,copy,true> $<TARGET_RUNTIME_DLLS:TestInstall> $<TARGET_FILE_DIR:TestInstall> COMMAND_EXPAND_LISTS)

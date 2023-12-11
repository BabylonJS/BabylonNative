# cmake 3.18+ to have the ARCHIVE_EXTRACT sub-command for files
cmake_minimum_required(VERSION 3.18)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

project(TestInstall)

FetchContent_Declare(cmake-extensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG 366dc4a84fb20f4060d97e89948c343e74c51fc3)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz")
FetchContent_MakeAvailable(cmake-extensions googletest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
    "${APPS_DIR}/node_modules/babylonjs/babylon.max.js.map"
    "${APPS_DIR}/node_modules/babylonjs-materials/babylonjs.materials.js"
    "${APPS_DIR}/node_modules/babylonjs-materials/babylonjs.materials.js.map"
    "${APPS_DIR}/node_modules/chai/chai.js"
    "${APPS_DIR}/node_modules/mocha/mocha.js")

set(HEADERS "${UNITTESTS_DIR}/Shared/Tests.h")

if(WIN32)
    set(TEST_INSTALL_APP "${UNITTESTS_DIR}/Win32/App.cpp")
else()
	message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

# Nuget/JSI
set(NUGET_PATH "${BINARY_DIR}/NuGet")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(V8JSI_VERSION "0.64.33")
    set(CPU_ARCH "x64")
    set(PLATFORM_FOLDER "win32")
    set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
    set(V8JSI_LIB_PATH_RELEASE "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Release/${CPU_ARCH}/")
    set(JSI_CPP "${V8JSI_PACKAGE_PATH}/build/native/jsi/jsi/jsi.cpp")
    set(ADDITIONAL_LIBRARIES "v8jsi.dll.lib")
elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    set_cpu_platform_arch()
    set(V8_VERSION "8.4.371.15")
    download_nuget()
    set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v142-${CPU_ARCH}.${V8_VERSION}")
    set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v142-${CPU_ARCH}.${V8_VERSION}")
    set(V8_REDIST_PACKAGE_PATH "${NUGET_PATH}/packages/v8.redist-v142-${CPU_ARCH}.${V8_VERSION}")

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
        "${V8_REDIST_PACKAGE_PATH}/lib/Release/icui18n.dll"
        "${V8_REDIST_PACKAGE_PATH}/lib/Release/icuuc.dll"
        "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8.dll"
        "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libbase.dll"
        "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libplatform.dll"
        "${V8_REDIST_PACKAGE_PATH}/lib/Release/zlib.dll")

    # only 1 imported location per library -> Adding 1 library per file
    foreach(V8FILE ${V8_DIST})
        get_filename_component(V8FILE_NAME "${V8FILE}" NAME_WE)
        add_library("v8::${V8FILE_NAME}" SHARED IMPORTED)
        set_target_properties("v8::${V8FILE_NAME}" PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libbase.dll.lib")
        set_target_properties("v8::${V8FILE_NAME}" PROPERTIES IMPORTED_LOCATION ${V8FILE})
    endforeach()

    set(ADDITIONAL_LIBRARIES
        v8
        v8inspector
        llhttp
        v8::icudtl
        v8::icui18n
        v8::icuuc
        v8::v8
        v8::v8_libbase
        v8::v8_libplatform
        v8::zlib)

    if(CPU_ARCH STREQUAL "x64")
        # Enable V8 Pointer Compression
        # https://v8.dev/blog/pointer-compression
        # https://stackoverflow.com/q/62921373
        set(NAPI_DEFINITIONS PUBLIC V8_COMPRESS_POINTERS)
    endif()
endif()

add_executable(TestInstall ${LOCAL_SCRIPTS} ${NPM_SCRIPTS} ${TEST_INSTALL_APP} ${HEADERS} ${JSI_CPP})

# for napi
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

set(INSTALL_LIBS_DIR "${BINARY_DIR}/install/lib")
target_include_directories(TestInstall PRIVATE "${BINARY_DIR}/install/include")
target_link_directories(TestInstall PRIVATE ${INSTALL_LIBS_DIR} ${V8JSI_LIB_PATH_RELEASE})

target_link_libraries(TestInstall
    AppRuntime
    arcana
    astc-encoder
    bgfx
    bimg
    bx
    Canvas
    Console
    edtaa3
    etc1
    etc2
    ExternalTexture
    GenericCodeGen
    glslang
    Graphics
    iqa
    JsRuntime
    MachineIndependent
    napi
    NativeCamera
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    NativeXr
    nvtt
    OGLCompiler
    openxr_loader
    OSDependent
    pvrtc
    ScriptLoader
    spirv-cross-core
    spirv-cross-hlsl
    SPIRV
    squish
    tinyexr
    UrlLib
    Window
    XMLHttpRequest
    chakrart
    d3d11
    d3d12
    d3dcompiler
    Pathcch
    glslang-default-resource-limits
    gtest_main
    ${ADDITIONAL_LIBRARIES}
    )

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET TestInstall POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:TestInstall>>,copy,true> $<TARGET_RUNTIME_DLLS:TestInstall> $<TARGET_FILE_DIR:TestInstall> COMMAND_EXPAND_LISTS)

# cmake 3.18+ to have the ARCHIVE_EXTRACT sub-command for files
cmake_minimum_required(VERSION 3.18)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(TestInstall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837
if (MSVC)
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

add_executable(TestInstall ${LOCAL_SCRIPTS} ${NPM_SCRIPTS} ${TEST_INSTALL_APP} ${HEADERS})

# for napi
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

set(INSTALL_LIBS_DIR "${BINARY_DIR}/install/lib")
target_include_directories(TestInstall PRIVATE "${BINARY_DIR}/install/include")
target_link_directories(TestInstall PRIVATE ${INSTALL_LIBS_DIR})

# Nuget/JSI
# TODO: move it to generic cmake
set(NUGET_PATH "${BINARY_DIR}/NuGet")
set(V8JSI_VERSION "0.64.33")
set(CPU_ARCH "x64")
set(PLATFORM_FOLDER "win32")
set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
set(V8JSI_LIB_PATH_RELEASE "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Release/${CPU_ARCH}/")
target_link_directories(TestInstall PRIVATE "${V8JSI_LIB_PATH_RELEASE}")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(EXTRA_LIBS "v8jsi.dll.lib")
endif()

set(STD_LIBS AppRuntime
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
#    NativeXr
    nvtt
    OGLCompiler
    openxr_loader
    OSDependent
    pvrtc
    ScriptLoader
    spirv-cross-core
    spirv-cross-glsl
    spirv-cross-hlsl
    SPIRV
    squish
    tinyexr
    UrlLib
    Window
    XMLHttpRequest
    )

target_link_libraries(TestInstall
    ${STD_LIBS}
    ${EXTRA_LIBS}
    chakrart
    d3d11
    d3d12
    d3dcompiler
    Pathcch
    )
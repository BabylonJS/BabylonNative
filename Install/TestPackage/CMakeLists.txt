# cmake 3.18+ to have the ARCHIVE_EXTRACT sub-command for files
cmake_minimum_required(VERSION 3.18)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set fetched URL contents to the time of extraction, not the timestamps in the archive.
if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()
# An imported target missing its location property fails during generation.
if(POLICY CMP0111)
    cmake_policy(SET CMP0111 NEW)
endif()

include(FetchContent)

project(TestPackage)

FetchContent_Declare(cmake-extensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG 366dc4a84fb20f4060d97e89948c343e74c51fc3)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz")
FetchContent_Declare(babylonnative
    URL "https://github.com/CedricGuillemet/BabylonNative/releases/download/V${BABYLON_NATIVE_TAG}/ReleasePackage.tar.gz")
FetchContent_MakeAvailable(cmake-extensions babylonnative googletest)

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

set(APPS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../Apps")
set(UNITTESTS_DIR "${APPS_DIR}/UnitTests")
set(LOCAL_SCRIPTS "${UNITTESTS_DIR}/Scripts/tests.js")

#npm(install --silent WORKING_DIRECTORY ${APPS_DIR})

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

add_executable(TestPackage ${LOCAL_SCRIPTS} ${NPM_SCRIPTS} ${TEST_INSTALL_APP} ${HEADERS} ${JSI_CPP})

# for napi
target_compile_definitions(TestPackage PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(TestPackage PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

set(INSTALL_LIBS_DIR "${BINARY_DIR}/install/lib")
target_include_directories(TestPackage PRIVATE "${BINARY_DIR}/install/include")
target_link_directories(TestPackage PRIVATE ${INSTALL_LIBS_DIR} ${V8JSI_LIB_PATH_RELEASE})

target_link_libraries(TestPackage
    babylonnative_static
    AppRuntime
    arcana
    Canvas
    Console
    ExternalTexture
    Graphics
    JsRuntime
    napi
    NativeCamera
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    NativeXr
    ScriptLoader
    UrlLib
    Window
    XMLHttpRequest
    chakrart
    d3d11
    d3d12
    d3dcompiler
    Pathcch
    gtest_main
    ${ADDITIONAL_LIBRARIES}
    )

# See https://gitlab.kitware.com/cmake/cmake/-/issues/23543
# If we can set minimum required to 3.26+, then we can use the `copy -t` syntax instead.
add_custom_command(TARGET TestPackage POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E $<IF:$<BOOL:$<TARGET_RUNTIME_DLLS:TestPackage>>,copy,true> $<TARGET_RUNTIME_DLLS:TestPackage> $<TARGET_FILE_DIR:TestPackage> COMMAND_EXPAND_LISTS)

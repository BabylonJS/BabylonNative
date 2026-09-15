cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# The RNW project generates projections with its resolved Microsoft.Windows.CppWinRT
# package. Reuse that exact tree for every CMake target that can enter the final link.
if(NOT DEFINED CPPWINRT_INCLUDE_PATH OR
   NOT EXISTS "${CPPWINRT_INCLUDE_PATH}/winrt/base.h")
    message(FATAL_ERROR
        "CPPWINRT_INCLUDE_PATH must point to the RNW project's generated C++/WinRT "
        "projection directory (the directory containing winrt/base.h).")
endif()

project(ReactNativeBabylon)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# BabylonNative CMake entry
set(BABYLON_NATIVE_BUILD_APPS OFF CACHE BOOL "")
set(BABYLON_NATIVE_USE_SWAPCHAINPANEL ON CACHE BOOL "")

message(STATUS "Using C++/WinRT projections from ${CPPWINRT_INCLUDE_PATH}")
include_directories(BEFORE "${CPPWINRT_INCLUDE_PATH}")

# Configure Babylon Native to use JSI
# Note: We should avoid installing node_modules in the Module\@babylonjs\react-native folder.
# Installing react-native dependencies for both the Playground app and the @babylonjs/react-native package will generate a bad bundle/runtime errors.
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "")

set(PLAYGROUND_DIR "${CMAKE_CURRENT_LIST_DIR}/../../..")

if(EXISTS "${PLAYGROUND_DIR}/react-native/package.json")
    get_filename_component(REACTNATIVE_DIR_CMAKE "${PLAYGROUND_DIR}/react-native" ABSOLUTE)
else()
    message("Playground directory ${PLAYGROUND_DIR}")
    message(FATAL_ERROR "No Playground available")
endif()

add_subdirectory("${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi" ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Disable Unity build for UrlLib because of conflict in header between windows.h and winrt
set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)

add_library(BabylonNative ${SHARED_SOURCES})

target_compile_definitions(BabylonNative PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEXR=${BABYLON_NATIVE_PLUGIN_NATIVEXR})
target_compile_definitions(BabylonNative PRIVATE BABYLON_NATIVE_PLUGIN_NATIVECAMERA=${BABYLON_NATIVE_PLUGIN_NATIVECAMERA})
target_include_directories(BabylonNative PRIVATE ${SHARED_INCLUDES})

# because of warning C5030: attribute 'msvc::intrinsic' is not recognized
# solution is to update VS version but this might bring more issues
disable_warnings(Graphics)
disable_warnings(NativeEngine)
disable_warnings(NativeCamera)
disable_warnings(NativeInput)
disable_warnings(NativeCapture)
disable_warnings(Canvas)
disable_warnings(Window)
disable_warnings(ExternalTexture)

target_link_libraries(BabylonNative
    arcana
    GraphicsDevice
    jsi
    JsRuntime
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    Window
    Scheduling
    XMLHttpRequest
    ShaderCache
    Canvas
    ${ADDITIONAL_LIBRARIES})

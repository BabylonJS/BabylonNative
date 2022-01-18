cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ReactNativeBabylon)

set(CMAKE_CXX_STANDARD 17)

set(BABYLON_REACT_NATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../react-native")
include(${BABYLON_REACT_NATIVE_DIR}/shared/CMakeLists.txt)

# Configure Babylon Native to use JSI
# Note: We should avoid installing node_modules in the Module\@babylonjs\react-native folder.
# Installing react-native dependencies for both the Playground app and the @babylonjs\react-native package will generate a bad bundle/runtime errors.
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "")
set(REACTNATIVE_DIR_CMAKE "${CMAKE_CURRENT_LIST_DIR}/../../../../Apps/Playground/node_modules/react-native")
add_subdirectory("${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi" ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

set(BABYLON_NATIVE_BUILD_APPS OFF CACHE BOOL "")
set(BABYLON_NATIVE_USE_SWAPCHAINPANEL ON CACHE BOOL "")
set(BABYLON_NATIVE_DIR "${BABYLON_REACT_NATIVE_DIR}/submodules/BabylonNative")
add_subdirectory(${BABYLON_NATIVE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/BabylonNative EXCLUDE_FROM_ALL)

add_library(BabylonNative
    ${SHARED_SOURCES})

target_include_directories(BabylonNative PRIVATE ${SHARED_INCLUDES})

target_link_libraries(BabylonNative
    arcana
    Graphics
    jsi
    JsRuntime
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    NativeXr
    Window
    XMLHttpRequest
    Canvas)

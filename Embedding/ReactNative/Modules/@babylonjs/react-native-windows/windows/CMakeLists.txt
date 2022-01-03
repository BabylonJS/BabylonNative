# cmake 3.15.3+ because of a c++ regression when swift is enable
cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ReactNativeBabylon)

set(BABYLON_NATIVE_PLATFORM "UWP")
set(CMAKE_SYSTEM_NAME "WindowsStore")
set(CMAKE_SYSTEM_VERSION "10.0")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BABYLON_REACT_NATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../react-native")
set(BABYLON_NATIVE_WINDOWS_STORE_USE_SWAPCHAINPANEL 1)

if (NOT GRAPHICS_API OR GRAPHICS_API STREQUAL "")
    set (GRAPHICS_API "D3D11")
    message("No GRAPHICS_API specified. Defaulting to D3D11.")
endif()

include(${BABYLON_REACT_NATIVE_DIR}/shared/CMakeLists.txt)

set(BABYLON_NATIVE_DIR "${BABYLON_REACT_NATIVE_DIR}/submodules/BabylonNative")

# Configure Babylon Native to use JSI
# Note: We should avoid installing node_modules in the Module\@babylonjs\react-native folder.
# Installing react-native dependencies for both the Playground app and the @babylonjs\react-native package will generate a bad bundle/runtime errors.
set(PLAYGROUND_NODE_MODULES_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../Apps/Playground/node_modules")
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "The JavaScript engine to power N-API")
set(REACTNATIVE_DIR_CMAKE "${PLAYGROUND_NODE_MODULES_DIR}/react-native")
add_subdirectory("${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi" ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

add_subdirectory("${BABYLON_NATIVE_DIR}/Dependencies" ${CMAKE_CURRENT_BINARY_DIR}/Dependencies EXCLUDE_FROM_ALL)
add_subdirectory("${BABYLON_NATIVE_DIR}/Core" ${CMAKE_CURRENT_BINARY_DIR}/Core EXCLUDE_FROM_ALL)
add_subdirectory("${BABYLON_NATIVE_DIR}/Plugins" ${CMAKE_CURRENT_BINARY_DIR}/Plugins EXCLUDE_FROM_ALL)
add_subdirectory("${BABYLON_NATIVE_DIR}/Polyfills" ${CMAKE_CURRENT_BINARY_DIR}/Polyfills EXCLUDE_FROM_ALL)

add_library(BabylonNative ${SHARED_SOURCES})
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

if(DEFINED EXTENSIONS_DIRS)
    foreach(extensionDir IN LISTS EXTENSIONS_DIRS)
        add_subdirectory(${extensionDir} "Extensions" EXCLUDE_FROM_ALL)
    endforeach()
endif()

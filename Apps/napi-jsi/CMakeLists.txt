# cmake 3.15.3+ because of a c++ regression when swift is enable
cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(napi-jsi)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BABYLON_NATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../")
set(BABYLON_NATIVE_WINDOWS_STORE_USE_SWAPCHAINPANEL 1)

# Configure Babylon Native to use JSI
set(NODE_MODULES_DIR "${CMAKE_CURRENT_LIST_DIR}/node_modules")
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "The JavaScript engine to power N-API")
set(REACTNATIVE_DIR_CMAKE "${NODE_MODULES_DIR}/react-native")
add_subdirectory("${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi" ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

add_subdirectory("${BABYLON_NATIVE_DIR}/Dependencies" ${CMAKE_CURRENT_BINARY_DIR}/Dependencies EXCLUDE_FROM_ALL)
add_subdirectory("${BABYLON_NATIVE_DIR}/Core" ${CMAKE_CURRENT_BINARY_DIR}/Core EXCLUDE_FROM_ALL)
add_subdirectory("${BABYLON_NATIVE_DIR}/Plugins" ${CMAKE_CURRENT_BINARY_DIR}/Plugins EXCLUDE_FROM_ALL)
add_subdirectory("${BABYLON_NATIVE_DIR}/Polyfills" ${CMAKE_CURRENT_BINARY_DIR}/Polyfills EXCLUDE_FROM_ALL)

# Setup a empty project that references everything we care about building against the jsi version of napi
add_library(napi-jsi empty.cpp)
target_include_directories(napi-jsi PRIVATE)
target_link_libraries(napi-jsi
    arcana
    Graphics
    jsi
    JsRuntime
    NativeCapture
    NativeEngine
    NativeInput
    NativeXr
    Window
    XMLHttpRequest)
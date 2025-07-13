cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ReactNativeBabylon)

# Disable the WebP plugin option
set(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP OFF CACHE BOOL "Disable Babylon Native WebP Support")

set(CMAKE_CXX_STANDARD 17)

set(BABYLON_REACT_NATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../react-native")
set(BABYLON_REACT_NATIVE_IOSANDROID "${CMAKE_CURRENT_LIST_DIR}/../../react-native-iosandroid")

include(${BABYLON_REACT_NATIVE_DIR}/shared/CMakeLists.txt)

# Configure Babylon Native to use JSI
# Note: We should avoid installing node_modules in the Module\@babylonjs\react-native folder.
# Installing react-native dependencies for both the Playground app and the @babylonjs\react-native package will generate a bad bundle/runtime errors.
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "")

set(PLAYGROUND_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../Apps/Playground/Playground")
set(PLAYGROUND_RNTA_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../Apps/BRNPlayground")

if(EXISTS "${PLAYGROUND_DIR}/node_modules/react-native/package.json")
    get_filename_component(REACTNATIVE_DIR_CMAKE "${PLAYGROUND_DIR}/node_modules/react-native" ABSOLUTE)
elseif(EXISTS "${PLAYGROUND_RNTA_DIR}/node_modules/react-native/package.json")
    get_filename_component(REACTNATIVE_DIR_CMAKE "${PLAYGROUND_RNTA_DIR}/node_modules/react-native" ABSOLUTE)
else()
    message(FATAL_ERROR "No Playground available")
endif()

add_subdirectory("${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi" ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

set(BABYLON_NATIVE_BUILD_APPS OFF CACHE BOOL "")
set(BABYLON_NATIVE_USE_SWAPCHAINPANEL ON CACHE BOOL "")
add_subdirectory(${babylonnative_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/BabylonNative EXCLUDE_FROM_ALL)
# Disable Unity build for UrlLib because of conflict in header between windows.h and winrt
set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)

add_library(BabylonNative
    ${SHARED_SOURCES})

if (${BASEKIT_BUILD})
    target_compile_definitions(BabylonNative PRIVATE BASEKIT_BUILD=1)
else()
    set(ADDITIONAL_LIBRARIES NativeXr NativeCamera)
endif()

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
    Canvas
    ${ADDITIONAL_LIBRARIES})


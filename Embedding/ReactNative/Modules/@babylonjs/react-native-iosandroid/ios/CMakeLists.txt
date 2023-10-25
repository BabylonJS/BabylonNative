cmake_minimum_required(VERSION 3.13.2)

include(FetchContent)

FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 04d91f6675dabb3c97df346a32f6184b0a7ef845)

FetchContent_MakeAvailable(ios-cmake)
set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
set(PLATFORM "OS64COMBINED" CACHE STRING "")
set(DEPLOYMENT_TARGET "12" CACHE STRING "")
set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
set(ENABLE_PCH OFF CACHE STRING "Enables or disables precompiled headers.")

project(ReactNativeBabylon)

include(${CMAKE_CURRENT_LIST_DIR}/../../react-native/shared/CMakeLists.txt)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")

# configure Babylon Native to use JSI
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "The JavaScript engine to power N-API")

get_filename_component(REACTNATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../react-native" ABSOLUTE)
if(NOT EXISTS ${REACTNATIVE_DIR})
    get_filename_component(REACTNATIVE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../Apps/Playground/Playground/node_modules/react-native" ABSOLUTE)
endif()

# The CMake generator for XCode produces relative paths to source files. Additionally, when an XCode project
# that lives in a sym linked directory is added to an XCode workspace, XCode resolves the actual path of the
# XCode project. This combination results in incorrect relative paths, so just copy the jsi directory locally.
file(COPY ${REACTNATIVE_DIR}/ReactCommon/jsi/jsi DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/jsi)
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/jsi/jsi ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR}/ReactCommon/jsi)

add_library(reactnative INTERFACE)
target_include_directories(reactnative INTERFACE ${REACTNATIVE_DIR}/ReactCommon/callinvoker)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/")

set(BABYLON_NATIVE_BUILD_APPS OFF CACHE BOOL "")
add_subdirectory(${babylonnative_SOURCE_DIR} ${babylonnative_SOURCE_DIR}/build/ios)

set(BABYLON_REACT_NATIVE_SHARED_DIR "${CMAKE_CURRENT_LIST_DIR}/../../react-native/shared")
add_subdirectory(${BABYLON_REACT_NATIVE_SHARED_DIR} ${CMAKE_CURRENT_BINARY_DIR}/shared)

add_library(BabylonNative
    ${SHARED_SOURCES})

if (${BASEKIT_BUILD})
    target_compile_definitions(BabylonNative PRIVATE BASEKIT_BUILD=1)
else()
    set(ADDITIONAL_LIBRARIES NativeXr NativeCamera)
endif()

target_include_directories(BabylonNative PRIVATE ${SHARED_INCLUDES})
target_include_directories(BabylonNative PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(BabylonNative
    z
    arcana
    GraphicsDevice
    jsi
    reactnative
    JsRuntime
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    Window
    XMLHttpRequest
    Canvas
    ${ADDITIONAL_LIBRARIES})

# TODO: For some reason these don't work, so we specify these in the CMake command line args.
set_target_properties(BabylonNative PROPERTIES
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC NO
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET 12.0
    XCODE_ATTRIBUTE_ENABLE_BITCODE YES
)
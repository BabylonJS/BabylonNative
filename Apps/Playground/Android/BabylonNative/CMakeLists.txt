cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BabylonNative)

get_filename_component(PLAYGROUND_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
get_filename_component(REPO_ROOT_DIR "${PLAYGROUND_DIR}/../.." ABSOLUTE)

add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/BabylonNative")

add_library(BabylonNativeJNI SHARED
    src/main/cpp/BabylonNativeJNI.cpp
    ${PLAYGROUND_DIR}/Shared/AppContext.cpp)

target_include_directories(BabylonNativeJNI
    PRIVATE ${PLAYGROUND_DIR})

target_link_libraries(BabylonNativeJNI
    GLESv3
    android
    EGL
    log
    -lz
    AndroidExtensions
    AppRuntime
    Blob
    Canvas
    Console
    GraphicsDevice
    NativeCamera
    NativeCapture
    NativeEncoding
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    NativeXr
    ScriptLoader
    ShaderCache
    TestUtils
    Window
    XMLHttpRequest)

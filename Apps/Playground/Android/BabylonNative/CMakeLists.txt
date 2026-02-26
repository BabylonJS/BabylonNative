cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
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
    PRIVATE GLESv3
    PRIVATE android
    PRIVATE EGL
    PRIVATE log
    PRIVATE -lz
    PRIVATE AndroidExtensions
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Canvas
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE NativeCamera
    PRIVATE NativeCapture
    PRIVATE NativeEncoding
    PRIVATE NativeEngine
    PRIVATE NativeInput
    PRIVATE NativeOptimizations
    PRIVATE NativeTracing
    PRIVATE NativeXr
    PRIVATE Performance
    PRIVATE ScriptLoader
    PRIVATE ShaderCache
    PRIVATE TestUtils
    PRIVATE TextDecoder
    PRIVATE Window
    PRIVATE XMLHttpRequest)

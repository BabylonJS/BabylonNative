cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BabylonNativeAAR)

# Repo root: Install/AndroidAAR/BabylonNative/../../..
get_filename_component(REPO_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/BabylonNative")

add_library(BabylonNativeJNI SHARED
    src/main/cpp/BabylonNativeJNI.cpp)

target_link_libraries(BabylonNativeJNI
    PRIVATE GLESv3
    PRIVATE android
    PRIVATE EGL
    PRIVATE log
    PRIVATE -lz
    PRIVATE AndroidExtensions
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE NativeEngine
    PRIVATE Performance
    PRIVATE ScriptLoader
    PRIVATE ShaderCache
    PRIVATE Window)

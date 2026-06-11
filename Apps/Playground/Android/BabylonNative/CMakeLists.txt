cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(BabylonNative)

get_filename_component(PLAYGROUND_DIR "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
get_filename_component(REPO_ROOT_DIR "${PLAYGROUND_DIR}/../.." ABSOLUTE)

# Build the cross-platform Embedding facade *and* the Android JNI
# interop layer (`libBabylonNativeEmbedding.so`). The Playground
# Android app loads that library directly via `BabylonNative.java`
# (in the `com.babylonjs.embedding` package).
set(BABYLON_NATIVE_EMBEDDING ON CACHE BOOL "" FORCE)
set(BABYLON_NATIVE_EMBEDDING_ANDROID ON CACHE BOOL "" FORCE)

add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/BabylonNative")

# Append a Playground-specific JNI helper to the generic
# BabylonNativeEmbedding target so the bootstrap script list can stay
# in Apps/Playground/Shared/PlaygroundScripts.{h,cpp} (shared with the
# Win32 / iOS / macOS hosts) instead of being duplicated on the Java
# side. Compiling these into the same .so keeps the Embedding layer
# as a single in-process instance — handles produced by one entry point
# can be safely consumed by another.
target_sources(BabylonNativeEmbedding
    PRIVATE src/main/cpp/PlaygroundJNI.cpp
            ${PLAYGROUND_DIR}/Shared/Diagnostics.cpp
            ${PLAYGROUND_DIR}/Shared/PlaygroundScripts.cpp)

target_include_directories(BabylonNativeEmbedding
    PRIVATE ${PLAYGROUND_DIR})

target_link_libraries(BabylonNativeEmbedding
    PRIVATE bx
    PRIVATE Foundation)     # for <Babylon/PerfTrace.h> in PlaygroundScripts.cpp

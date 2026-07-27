# Babylon::Embedding Apple interop layer.
#
# Builds a static library producing Obj-C `BNRuntime` and `BNView`
# classes, importable from Swift via the standard Obj-C bridge.
# The host's Xcode project (or downstream CMake project) consumes
# this directly.
#
# Gated by BABYLON_NATIVE_EMBEDDING_APPLE at the root.

if(NOT APPLE)
    message(FATAL_ERROR
        "Embedding/Apple is for iOS / macOS / visionOS. "
        "Disable BABYLON_NATIVE_EMBEDDING_APPLE for non-Apple builds.")
endif()

set(APPLE_EMBEDDING_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Include/Platform/Apple")

set(SOURCES
    "${APPLE_EMBEDDING_INCLUDE_ROOT}/Babylon/Embedding/Apple/BabylonNativeEmbedding.h"
    "${APPLE_EMBEDDING_INCLUDE_ROOT}/Babylon/Embedding/Apple/BNRuntime.h"
    "${APPLE_EMBEDDING_INCLUDE_ROOT}/Babylon/Embedding/Apple/BNRuntimeNative.h"
    "${APPLE_EMBEDDING_INCLUDE_ROOT}/Babylon/Embedding/Apple/BNView.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNRuntime.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNRuntimeInternal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNView.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNViewDelegate.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/AudioSession.mm")

add_library(BabylonNativeEmbedding STATIC ${SOURCES})

warnings_as_errors(BabylonNativeEmbedding)

target_include_directories(BabylonNativeEmbedding
    PUBLIC "${APPLE_EMBEDDING_INCLUDE_ROOT}"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(BabylonNativeEmbedding
    PUBLIC Embedding
    PRIVATE WebAudio
    PRIVATE "-framework Foundation"
    PRIVATE "-framework QuartzCore"
    PRIVATE "-framework MetalKit"
    PRIVATE "-framework AVFoundation")

# Enable ARC for the Obj-C++ files.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNRuntime.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNView.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNViewDelegate.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/AudioSession.mm"
    PROPERTIES COMPILE_FLAGS "-fobjc-arc")

set_property(TARGET BabylonNativeEmbedding PROPERTY FOLDER Embedding)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${SOURCES})

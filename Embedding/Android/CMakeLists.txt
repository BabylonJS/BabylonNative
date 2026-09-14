# Babylon::Embedding Android interop layer.
#
# Builds a shared library (`libBabylonNativeEmbedding.so`) containing
# the JNI entry points declared in `BabylonNative.kt`. The host's
# Android Studio / Gradle project consumes this CMakeLists via its
# `externalNativeBuild { cmake { path "..." } }` hookup, alongside
# adding `src/main/java/` to its `sourceSets`.
#
# Gated by BABYLON_NATIVE_EMBEDDING_ANDROID at the root.

if(NOT ANDROID)
    message(FATAL_ERROR
        "Embedding/Android is Android-only. "
        "Disable BABYLON_NATIVE_EMBEDDING_ANDROID for non-Android builds.")
endif()

set(ANDROID_EMBEDDING_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Include/Platform/Android")

set(SOURCES
    "${ANDROID_EMBEDDING_INCLUDE_ROOT}/Babylon/Embedding/Android/RuntimeHandle.h"
    "${ANDROID_EMBEDDING_INCLUDE_ROOT}/Babylon/Embedding/Android/AudioPlatform.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/BabylonNativeEmbedding.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/AudioPlatformAndroid.cpp")

add_library(BabylonNativeEmbedding SHARED ${SOURCES})

warnings_as_errors(BabylonNativeEmbedding)

target_include_directories(BabylonNativeEmbedding PUBLIC "${ANDROID_EMBEDDING_INCLUDE_ROOT}")

target_link_libraries(BabylonNativeEmbedding
    PRIVATE Embedding
    PRIVATE WebAudio
    PRIVATE AndroidExtensions
    PRIVATE android   # ANativeWindow_fromSurface
    PRIVATE log
    PRIVATE EGL       # required by bgfx GL backend
    PRIVATE GLESv3
    PRIVATE -lz)

set_property(TARGET BabylonNativeEmbedding PROPERTY FOLDER Embedding)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${SOURCES})

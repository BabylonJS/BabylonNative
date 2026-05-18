# Babylon::Integrations Android interop layer.
#
# Builds a shared library (`libBabylonNativeIntegrations.so`) containing
# the JNI entry points declared in `BabylonNative.kt`. The host's
# Android Studio / Gradle project consumes this CMakeLists via its
# `externalNativeBuild { cmake { path "..." } }` hookup, alongside
# adding `src/main/java/` to its `sourceSets`.
#
# Gated by BABYLON_NATIVE_INTEGRATIONS_ANDROID at the root.

if(NOT ANDROID)
    message(FATAL_ERROR
        "Integrations/Android is Android-only. "
        "Disable BABYLON_NATIVE_INTEGRATIONS_ANDROID for non-Android builds.")
endif()

set(ANDROID_INTEGRATIONS_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Include/Platform/Android")

set(SOURCES
    "${ANDROID_INTEGRATIONS_INCLUDE_ROOT}/Babylon/Integrations/Android/RuntimeHandle.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/BabylonNativeIntegrations.cpp")

add_library(BabylonNativeIntegrations SHARED ${SOURCES})

warnings_as_errors(BabylonNativeIntegrations)

target_include_directories(BabylonNativeIntegrations PUBLIC "${ANDROID_INTEGRATIONS_INCLUDE_ROOT}")

target_link_libraries(BabylonNativeIntegrations
    PRIVATE Integrations
    PRIVATE AndroidExtensions
    PRIVATE android   # ANativeWindow_fromSurface
    PRIVATE log
    PRIVATE EGL       # required by bgfx GL backend
    PRIVATE GLESv3
    PRIVATE -lz)

set_property(TARGET BabylonNativeIntegrations PROPERTY FOLDER Integrations)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${SOURCES})

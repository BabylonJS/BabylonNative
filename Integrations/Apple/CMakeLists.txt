# Babylon::Integrations Apple interop layer.
#
# Builds a static library producing Obj-C `BNRuntime` and `BNView`
# classes, importable from Swift via the standard Obj-C bridge.
# The host's Xcode project (or downstream CMake project) consumes
# this directly.
#
# Gated by BABYLON_NATIVE_INTEGRATIONS_APPLE at the root.

if(NOT APPLE)
    message(FATAL_ERROR
        "Integrations/Apple is for iOS / macOS / visionOS. "
        "Disable BABYLON_NATIVE_INTEGRATIONS_APPLE for non-Apple builds.")
endif()

set(APPLE_INTEGRATIONS_INCLUDE_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../Include/Platform/Apple")

set(SOURCES
    "${APPLE_INTEGRATIONS_INCLUDE_ROOT}/Babylon/Integrations/Apple/BabylonNativeIntegrations.h"
    "${APPLE_INTEGRATIONS_INCLUDE_ROOT}/Babylon/Integrations/Apple/BNRuntime.h"
    "${APPLE_INTEGRATIONS_INCLUDE_ROOT}/Babylon/Integrations/Apple/BNRuntimeNative.h"
    "${APPLE_INTEGRATIONS_INCLUDE_ROOT}/Babylon/Integrations/Apple/BNView.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNRuntime.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNRuntimeInternal.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNView.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNViewDelegate.mm")

add_library(BabylonNativeIntegrations STATIC ${SOURCES})

warnings_as_errors(BabylonNativeIntegrations)

target_include_directories(BabylonNativeIntegrations
    PUBLIC "${APPLE_INTEGRATIONS_INCLUDE_ROOT}"
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/Source")

target_link_libraries(BabylonNativeIntegrations
    PUBLIC Integrations
    PRIVATE "-framework Foundation"
    PRIVATE "-framework QuartzCore"
    PRIVATE "-framework MetalKit")

# Enable ARC for the Obj-C++ files.
set_source_files_properties(
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNRuntime.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNView.mm"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/BNViewDelegate.mm"
    PROPERTIES COMPILE_FLAGS "-fobjc-arc")

set_property(TARGET BabylonNativeIntegrations PROPERTY FOLDER Integrations)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/.." FILES ${SOURCES})

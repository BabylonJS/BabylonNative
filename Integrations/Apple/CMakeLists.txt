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

set(SOURCES
    "include/BabylonNativeIntegrations/BabylonNativeIntegrations.h"
    "include/BabylonNativeIntegrations/BNRuntime.h"
    "include/BabylonNativeIntegrations/BNView.h"
    "Source/BNRuntime.mm"
    "Source/BNRuntimeInternal.h"
    "Source/BNView.mm")

add_library(BabylonNativeIntegrations STATIC ${SOURCES})

warnings_as_errors(BabylonNativeIntegrations)

target_include_directories(BabylonNativeIntegrations
    PUBLIC "include"
    PRIVATE "Source")

target_link_libraries(BabylonNativeIntegrations
    PRIVATE Integrations
    PRIVATE "-framework Foundation"
    PRIVATE "-framework QuartzCore"
    PRIVATE "-framework MetalKit")

# Enable ARC for the Obj-C++ files.
set_source_files_properties(
    "Source/BNRuntime.mm"
    "Source/BNView.mm"
    PROPERTIES COMPILE_FLAGS "-fobjc-arc")

set_property(TARGET BabylonNativeIntegrations PROPERTY FOLDER Integrations)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

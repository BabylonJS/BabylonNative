set(SOURCES
    "Include/Babylon/Dispatchable.h"
    "Include/Babylon/AppRuntime.h"
    "Source/AppRuntime.cpp"
    "Source/AppRuntimeImpl.h"
    "Source/AppRuntimeImpl.cpp"
    "Source/AppRuntimeImpl_${NAPI_JAVASCRIPT_ENGINE}.cpp"
    "Source/AppRuntimeImpl_${BABYLON_NATIVE_PLATFORM}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}")

add_library(AppRuntime ${SOURCES})
warnings_as_errors(AppRuntime)

target_include_directories(AppRuntime
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_include_directories(AppRuntime INTERFACE "/usr/include/webkitgtk-4.0/")
endif()

if(APPLE)
    set_property(TARGET AppRuntime PROPERTY UNITY_BUILD false)
endif()

target_link_to_dependencies(AppRuntime
    PRIVATE arcana
    PRIVATE JsRuntime)

target_compile_definitions(AppRuntime
    PRIVATE NOMINMAX)

set_property(TARGET AppRuntime PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})


set(SOURCES
    "Include/Babylon/AppRuntime.h"
    "Source/AppRuntime.cpp"
    "Source/AppRuntime${NAPI_JAVASCRIPT_ENGINE}.cpp"
    "Source/AppRuntime${BABYLON_NATIVE_PLATFORM}.cpp"
    "Source/Common.h"
    "Source/Common${BABYLON_NATIVE_PLATFORM}.cpp"
    "Source/WorkQueue.cpp"
    "Source/WorkQueue.h")

add_library(AppRuntime ${SOURCES})

target_include_directories(AppRuntime PRIVATE "Include/Babylon")
target_include_directories(AppRuntime INTERFACE "Include")

if(UNIX AND NOT APPLE AND NOT ANDROID)
    target_include_directories(AppRuntime INTERFACE "/usr/include/webkitgtk-4.0/")
    find_library(JSCORE_LIBRARY JavaScriptCore)
endif()

target_link_to_dependencies(AppRuntime
    PRIVATE arcana
    PUBLIC JsRuntime
    PUBLIC ${JSCORE_LIBRARY})

target_compile_definitions(AppRuntime
    PRIVATE NOMINMAX)

set_property(TARGET AppRuntime PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

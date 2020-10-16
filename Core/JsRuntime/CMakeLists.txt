set(SOURCES
    "Include/Babylon/JsRuntime.h"
    "Include/Babylon/JsRuntimeScheduler.h"
    "Source/JsRuntimeInternalState.h"
    "Source/JsRuntime.cpp")

add_library(JsRuntime ${SOURCES})
warnings_as_errors(JsRuntime)

target_include_directories(JsRuntime PRIVATE "Include/Babylon")
target_include_directories(JsRuntime PUBLIC "Include")

target_link_to_dependencies(JsRuntime
    PUBLIC napi
    PRIVATE arcana)

set_property(TARGET JsRuntime PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(JsRuntimeInternal INTERFACE)
target_include_directories(JsRuntimeInternal INTERFACE "Source")
target_link_to_dependencies(JsRuntimeInternal
    INTERFACE JsRuntime
    INTERFACE arcana)

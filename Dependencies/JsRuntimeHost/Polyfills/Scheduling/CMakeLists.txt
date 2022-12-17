set(SOURCES
    "Include/Babylon/Polyfills/Scheduling.h"
    "Source/TimeoutDispatcher.h"
    "Source/TimeoutDispatcher.cpp"
    "Source/Scheduling.h"
    "Source/Scheduling.cpp")

add_library(Scheduling ${SOURCES})
#warnings_as_errors(Scheduling)

target_include_directories(Scheduling
    PUBLIC "Include")

target_link_libraries(Scheduling
    PUBLIC napi
    PRIVATE JsRuntime)

set_property(TARGET Scheduling PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

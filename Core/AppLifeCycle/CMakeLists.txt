set(SOURCES
    "Include/Babylon/AppLifeCycle.h"
    "Source/AppLifeCycle.cpp")

add_library(AppLifeCycle ${SOURCES})

target_include_directories(AppLifeCycle PRIVATE "Include/Babylon")
target_include_directories(AppLifeCycle INTERFACE "Include")

target_link_to_dependencies(AppLifeCycle
    PUBLIC arcana
    PUBLIC JsRuntime)

set_property(TARGET AppLifeCycle PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/ConsolePolyfill.h"
    "Source/Console.cpp"
    "Source/Console.h")

add_library(ConsolePolyfill ${SOURCES})

target_include_directories(ConsolePolyfill PUBLIC "Include")

target_link_to_dependencies(ConsolePolyfill PUBLIC napi)

set_property(TARGET ConsolePolyfill PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

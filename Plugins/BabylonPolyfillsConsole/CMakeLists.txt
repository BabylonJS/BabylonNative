set(SOURCES
    "Include/Babylon/Polyfills/Console.h"
    "Source/Console.cpp"
    "Source/Console.h")

add_library(BabylonPolyfillsConsole ${SOURCES})

target_include_directories(BabylonPolyfillsConsole PUBLIC "Include")

target_link_to_dependencies(BabylonPolyfillsConsole PUBLIC napi)

set_property(TARGET BabylonPolyfillsConsole PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

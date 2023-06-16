set(SOURCES
    "Include/Babylon/Polyfills/Console.h"
    "Source/Console.cpp")

add_library(Console ${SOURCES})
warnings_as_errors(Console)

target_include_directories(Console PUBLIC "Include")

target_link_libraries(Console PUBLIC napi)

set_property(TARGET Console PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

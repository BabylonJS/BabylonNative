set(SOURCES
    "Include/Babylon/Polyfills/DOM.h"
    "Source/DOM.cpp")

add_library(DOM ${SOURCES})
warnings_as_errors(DOM)

target_include_directories(DOM PUBLIC "Include")

target_link_to_dependencies(DOM PUBLIC napi)

set_property(TARGET DOM PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

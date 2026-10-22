set(SOURCES
    "Include/Babylon/Polyfills/TextDecoder.h"
    "Source/TextDecoder.cpp")

add_library(TextDecoder ${SOURCES})
warnings_as_errors(TextDecoder)

target_include_directories(TextDecoder PUBLIC "Include")

target_link_libraries(TextDecoder
    PUBLIC napi
    PUBLIC Foundation)

set_property(TARGET TextDecoder PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

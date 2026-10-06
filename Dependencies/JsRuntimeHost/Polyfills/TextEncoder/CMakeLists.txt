set(SOURCES
    "Include/Babylon/Polyfills/TextEncoder.h"
    "Source/TextEncoder.cpp")

add_library(TextEncoder ${SOURCES})
warnings_as_errors(TextEncoder)

target_include_directories(TextEncoder PUBLIC "Include")

target_link_libraries(TextEncoder
    PUBLIC napi
    PUBLIC Foundation)

set_property(TARGET TextEncoder PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

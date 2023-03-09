set(SOURCES 
    ${SOURCES}
    "Include/Babylon/Polyfills/URL.h"
    "Source/URL.cpp"
    "Source/URL.h"
    "Source/URLSearchParams.cpp"
    "Source/URLSearchParams.h")

add_library(URL ${SOURCES})

target_include_directories(URL PUBLIC "Include")

target_link_libraries(URL
    PUBLIC JsRuntime)

set_property(TARGET URL PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Polyfills/Fetch.h"
    "Source/Fetch.h"
    "Source/Fetch.cpp")

add_library(Fetch ${SOURCES})
warnings_as_errors(Fetch)

target_include_directories(Fetch PUBLIC "Include")

target_link_libraries(Fetch
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE UrlLib)

set_property(TARGET Fetch PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

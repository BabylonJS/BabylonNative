set(response_file "${CMAKE_CURRENT_SOURCE_DIR}/Source/Response.js")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${response_file}")
file(READ "${response_file}" response_js)
# Keep each raw string below MSVC's per-literal limit.
string(REPLACE "\n" "\n)JSRHRESP\"\nR\"JSRHRESP(" response_js "${response_js}")
configure_file(Source/Response.h.in Generated/Response.h @ONLY)

set(SOURCES
    "Source/Response.js"
    "Source/Response.h.in"
    "Include/Babylon/Polyfills/Fetch.h"
    "Source/Fetch.h"
    "Source/Fetch.cpp")

add_library(Fetch ${SOURCES})
warnings_as_errors(Fetch)

target_include_directories(Fetch PUBLIC "Include" PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Generated")

target_link_libraries(Fetch
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE UrlLib)

set_property(TARGET Fetch PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

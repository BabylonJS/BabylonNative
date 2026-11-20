set(FETCH_POLYFILL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Source/FetchPolyfill.js")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${FETCH_POLYFILL_FILE}")
file(READ "${FETCH_POLYFILL_FILE}" FETCH_POLYFILL_SOURCE)

# MSVC caps a single string literal at 16,380 bytes (C2026), so the bundle is emitted as adjacent
# raw-string literals of at most 16,000 bytes, which the compiler concatenates back into one array.
# Parts abut with no separator so the split stays byte-transparent.
string(LENGTH "${FETCH_POLYFILL_SOURCE}" _fetch_length)
set(_fetch_offset 0)
set(FETCH_POLYFILL_LITERALS "")
while(_fetch_offset LESS _fetch_length)
    math(EXPR _fetch_remaining "${_fetch_length} - ${_fetch_offset}")
    if(_fetch_remaining LESS 16000)
        set(_fetch_take ${_fetch_remaining})
    else()
        set(_fetch_take 16000)
    endif()
    string(SUBSTRING "${FETCH_POLYFILL_SOURCE}" ${_fetch_offset} ${_fetch_take} _fetch_part)
    string(APPEND FETCH_POLYFILL_LITERALS "        R\"JSRHFETCH(${_fetch_part})JSRHFETCH\"\n")
    math(EXPR _fetch_offset "${_fetch_offset} + ${_fetch_take}")
endwhile()
configure_file(
    "Source/FetchScripts.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Generated/FetchScripts.h"
    @ONLY)

set(SOURCES
    "Include/Babylon/Polyfills/Fetch.h"
    "Readme.md"
    "Source/Fetch.h"
    "Source/Fetch.cpp"
    "Source/FetchPolyfill.js"
    "Source/FetchScripts.h.in")

add_library(Fetch ${SOURCES})
warnings_as_errors(Fetch)

target_include_directories(Fetch
    PUBLIC "Include"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Generated")

target_link_libraries(Fetch
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE UrlLib
    PRIVATE URLInternal)

set_property(TARGET Fetch PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

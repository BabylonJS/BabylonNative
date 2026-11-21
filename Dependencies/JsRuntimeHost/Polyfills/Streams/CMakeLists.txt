set(WEB_STREAMS_POLYFILL_FILE
    "${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty/web-streams-polyfill/ponyfill.es5.js")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
    "${WEB_STREAMS_POLYFILL_FILE}")
file(READ "${WEB_STREAMS_POLYFILL_FILE}" WEB_STREAMS_POLYFILL_SOURCE)
# MSVC caps a single string literal at 16,380 bytes (C2026) and a concatenated one at 65,535, so
# the bundle is emitted as consecutive raw-string parts of at most 16,000 bytes and joined back
# into one array at compile time. Parts abut with no separator so the split stays byte-transparent
# (a separator could land inside an identifier or string literal when the bundle is regenerated).
string(LENGTH "${WEB_STREAMS_POLYFILL_SOURCE}" _streams_length)
set(_streams_part_size 16000)
set(_streams_offset 0)
set(_streams_index 0)
set(WEB_STREAMS_POLYFILL_PARTS "")
set(WEB_STREAMS_POLYFILL_PART_NAMES "")
while(_streams_offset LESS _streams_length)
    math(EXPR _streams_index "${_streams_index} + 1")
    math(EXPR _streams_remaining "${_streams_length} - ${_streams_offset}")
    if(_streams_remaining LESS _streams_part_size)
        set(_streams_take ${_streams_remaining})
    else()
        set(_streams_take ${_streams_part_size})
    endif()
    string(SUBSTRING "${WEB_STREAMS_POLYFILL_SOURCE}" ${_streams_offset} ${_streams_take} _streams_part)
    string(APPEND WEB_STREAMS_POLYFILL_PARTS
        "    inline constexpr char PonyfillPart${_streams_index}[] = R\"JSRHSTREAM(${_streams_part})JSRHSTREAM\";\n")
    string(APPEND WEB_STREAMS_POLYFILL_PART_NAMES ", PonyfillPart${_streams_index}")
    math(EXPR _streams_offset "${_streams_offset} + ${_streams_take}")
endwhile()
configure_file(
    "Source/StreamsScripts.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/Generated/StreamsScripts.h"
    @ONLY)

set(SOURCES
    "Include/Babylon/Polyfills/Streams.h"
    "Source/Streams.cpp"
    "Source/StreamsScripts.h.in"
    "ThirdParty/web-streams-polyfill/LICENSE"
    "ThirdParty/web-streams-polyfill/ponyfill.es5.js")

add_library(Streams ${SOURCES})
warnings_as_errors(Streams)

target_include_directories(Streams
    PUBLIC "Include"
    PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/Generated")

target_link_libraries(Streams PUBLIC JsRuntime)

set_property(TARGET Streams PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Polyfills/DecompressionStream.h"
    "Source/DecompressionStream.cpp"
    "Source/DecompressionStream.h"
    "Source/TextDecoder.h"
    "Source/ReadableStream.h"
    "Source/Response.h")

add_library(DecompressionStream ${SOURCES})
warnings_as_errors(DecompressionStream)

target_include_directories(DecompressionStream PUBLIC
    "Include")

target_link_libraries(DecompressionStream
    PUBLIC napi
    PRIVATE libdeflate_static
    PRIVATE minz
    PRIVATE JsRuntimeInternal)

set_property(TARGET DecompressionStream PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

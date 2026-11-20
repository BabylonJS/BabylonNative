set(SOURCES 
    ${SOURCES}
    "Include/Babylon/Polyfills/URL.h"
    "InternalInclude/Babylon/Polyfills/DataUrl.h"
    "Source/DataUrl.cpp"
    "Source/URL.cpp"
    "Source/URL.h"
    "Source/URLSearchParams.cpp"
    "Source/URLSearchParams.h")

add_library(URL ${SOURCES})

target_include_directories(URL PUBLIC "Include")
target_include_directories(URL PRIVATE "InternalInclude")

target_link_libraries(URL
    PRIVATE BlobInternal
    PRIVATE UrlLib
    PUBLIC JsRuntime)

set_property(TARGET URL PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Seam for other polyfills in this repo that need the data: URL decoder (the fetch polyfill keeps its
# synchronous fast path on it). Not part of the public "Include" directory, like BlobInternal.
add_library(URLInternal INTERFACE)
target_include_directories(URLInternal INTERFACE "InternalInclude")
target_link_libraries(URLInternal INTERFACE URL)

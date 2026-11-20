set(SOURCES
    "Include/Babylon/Polyfills/Worker.h"
    "Source/Worker.cpp"
    "Source/Worker.h"
    "Source/WorkerScripts.h")

add_library(Worker ${SOURCES})
warnings_as_errors(Worker)

target_include_directories(Worker PUBLIC "Include")

target_link_libraries(Worker
    PUBLIC AppRuntime
    PRIVATE AbortController
    PRIVATE Blob
    PRIVATE Compression
    PRIVATE Console
    PRIVATE Fetch
    PRIVATE File
    PRIVATE IndexedDB
    PRIVATE Performance
    PRIVATE Scheduling
    PRIVATE Streams
    PRIVATE TextDecoder
    PRIVATE TextEncoder
    PRIVATE URL
    PRIVATE WebSocket
    PRIVATE XMLHttpRequest)

set_property(TARGET Worker PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

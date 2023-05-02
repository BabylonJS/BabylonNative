set(SOURCES 
    ${SOURCES}
    "Include/Babylon/Polyfills/WebSocket.h"
    "Source/WebSocket.cpp"
    "Source/WebSocket.h")

add_library(WebSocket ${SOURCES})
#warnings_as_errors(WebSocket)

target_include_directories(WebSocket PUBLIC "Include")

target_link_libraries(WebSocket
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE UrlLib)

set_property(TARGET WebSocket PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

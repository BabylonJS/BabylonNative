if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

if(JSRUNTIMEHOST_POLYFILL_SCHEDULING)
    add_subdirectory(Scheduling)
endif()

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

if(JSRUNTIMEHOST_POLYFILL_URL)
    add_subdirectory(URL)
endif()

if(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER)
    add_subdirectory(AbortController)
endif()

if(JSRUNTIMEHOST_POLYFILL_WEBSOCKET)
    add_subdirectory(WebSocket)
endif()

if(JSRUNTIMEHOST_POLYFILL_BLOB)
    add_subdirectory(Blob)
endif()

if(JSRUNTIMEHOST_POLYFILL_PERFORMANCE)
    add_subdirectory(Performance)
endif()

if(JSRUNTIMEHOST_POLYFILL_TEXTDECODER)
    add_subdirectory(TextDecoder)
endif()
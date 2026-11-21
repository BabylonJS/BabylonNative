if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

if(JSRUNTIMEHOST_POLYFILL_SCHEDULING)
    add_subdirectory(Scheduling)
endif()

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

if(JSRUNTIMEHOST_POLYFILL_FETCH)
    add_subdirectory(Fetch)
endif()

if(JSRUNTIMEHOST_POLYFILL_URL)
    add_subdirectory(URL)
endif()

if(JSRUNTIMEHOST_POLYFILL_ABORTCONTROLLER)
    add_subdirectory(AbortController)
endif()

if(JSRUNTIMEHOST_POLYFILL_WEBSOCKET)
    add_subdirectory(WebSocket)
endif()

if(JSRUNTIMEHOST_POLYFILL_BLOB)
    add_subdirectory(Blob)
endif()

if(JSRUNTIMEHOST_POLYFILL_FILE)
    add_subdirectory(File)
endif()

if(JSRUNTIMEHOST_POLYFILL_PERFORMANCE)
    add_subdirectory(Performance)
endif()

if(JSRUNTIMEHOST_POLYFILL_TEXTDECODER)
    add_subdirectory(TextDecoder)
endif()

if(JSRUNTIMEHOST_POLYFILL_TEXTENCODER)
    add_subdirectory(TextEncoder)
endif()

if(JSRUNTIMEHOST_POLYFILL_WORKER)
    if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
        # The legacy JSI adapter only implements the v5-shaped C++ surface and
        # has no detachable ArrayBuffer ABI. Keep existing JSI configurations
        # buildable while the v7-capable engine adapters expose Worker.
        message(STATUS "Worker polyfill is unavailable with the legacy JSI adapter")
    else()
        set(_worker_required_polyfills
            JSRUNTIMEHOST_POLYFILL_CONSOLE
            JSRUNTIMEHOST_POLYFILL_SCHEDULING
            JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST
            JSRUNTIMEHOST_POLYFILL_FETCH
            JSRUNTIMEHOST_POLYFILL_URL
            JSRUNTIMEHOST_POLYFILL_ABORTCONTROLLER
            JSRUNTIMEHOST_POLYFILL_WEBSOCKET
            JSRUNTIMEHOST_POLYFILL_BLOB
            JSRUNTIMEHOST_POLYFILL_FILE
            JSRUNTIMEHOST_POLYFILL_PERFORMANCE
            JSRUNTIMEHOST_POLYFILL_TEXTDECODER
            JSRUNTIMEHOST_POLYFILL_TEXTENCODER)
        foreach(_worker_dependency IN LISTS _worker_required_polyfills)
            if(NOT ${_worker_dependency})
                message(FATAL_ERROR
                    "JSRUNTIMEHOST_POLYFILL_WORKER requires ${_worker_dependency}=ON so Worker realms expose the browser baseline.")
            endif()
        endforeach()
        add_subdirectory(Worker)
    endif()
endif()

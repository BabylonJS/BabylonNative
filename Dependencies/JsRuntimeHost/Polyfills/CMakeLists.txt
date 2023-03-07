if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

if(JSRUNTIMEHOST_POLYFILL_SCHEDULING)
    add_subdirectory(Scheduling)
endif()

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

if(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER)
    add_subdirectory(AbortController)
endif()

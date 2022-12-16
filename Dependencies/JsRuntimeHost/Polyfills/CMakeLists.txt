if(JSRUNTIMEHOST_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

# Window should probably be renamed to Timeout
if(JSRUNTIMEHOST_POLYFILL_WINDOW)
    add_subdirectory(Window)
endif()

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

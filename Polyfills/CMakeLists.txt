if(BABYLON_NATIVE_POLYFILL_CONSOLE)
    add_subdirectory(Console)
endif()

if(BABYLON_NATIVE_POLYFILL_WINDOW)
    add_subdirectory(Window)
endif()

if(BABYLON_NATIVE_POLYFILL_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

if(BABYLON_NATIVE_POLYFILL_CANVAS)
    add_subdirectory(Canvas)
endif()

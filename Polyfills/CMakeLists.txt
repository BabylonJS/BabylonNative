if(BABYLON_NATIVE_PLUGIN_CONSOLE)
    add_subdirectory(Console)
endif()

if(BABYLON_NATIVE_PLUGIN_WINDOW)
    add_subdirectory(Window)
endif()

if(BABYLON_NATIVE_PLUGIN_XMLHTTPREQUEST)
    add_subdirectory(XMLHttpRequest)
endif()

if(BABYLON_NATIVE_PLUGIN_CANVAS)
    add_subdirectory(Canvas)
endif()

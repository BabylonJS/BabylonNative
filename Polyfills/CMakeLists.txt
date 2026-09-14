if(BABYLON_NATIVE_POLYFILL_WINDOW)
    add_subdirectory(Window)
endif()

if(BABYLON_NATIVE_POLYFILL_CANVAS)
    add_subdirectory(Canvas)
endif()

if(BABYLON_NATIVE_POLYFILL_WEBAUDIO)
    add_subdirectory(WebAudio)
endif()

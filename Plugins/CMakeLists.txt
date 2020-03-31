add_subdirectory(ConsolePolyfill)
add_subdirectory(WindowPolyfill)
add_subdirectory(ScriptLoader)
add_subdirectory(XMLHttpRequest)

# Add NativeEnginePlugin
# Dependencies: WindowPolyfill
add_subdirectory(NativeEnginePlugin)

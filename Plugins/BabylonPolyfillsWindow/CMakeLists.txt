set(SOURCES
    "Include/Babylon/Polyfills/Window.h"
    "Source/NativeWindow.h"
    "Source/NativeWindow.cpp")

add_library(BabylonPolyfillsWindow ${SOURCES})

target_include_directories(BabylonPolyfillsWindow
    PUBLIC "Include")

target_link_to_dependencies(BabylonPolyfillsWindow 
    PUBLIC napi
    PRIVATE base-n
    PRIVATE BabylonNativeUtils
    PRIVATE JsRuntime)

set_property(TARGET BabylonPolyfillsWindow PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(BabylonPolyfillsWindowInternal INTERFACE)
target_include_directories(BabylonPolyfillsWindowInternal INTERFACE "Source")
target_link_to_dependencies(BabylonPolyfillsWindowInternal
    INTERFACE BabylonPolyfillsWindow
    INTERFACE BabylonNativeUtils
    INTERFACE JsRuntime)

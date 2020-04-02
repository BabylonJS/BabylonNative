set(SOURCES
    "Include/Babylon/Polyfills/Window.h"
    "Source/NativeWindow.h"
    "Source/NativeWindow.cpp")

add_library(Window ${SOURCES})

target_include_directories(Window
    PUBLIC "Include")

target_link_to_dependencies(Window 
    PUBLIC napi
    PRIVATE base-n
    PRIVATE BabylonNativeUtils
    PRIVATE JsRuntime)

set_property(TARGET Window PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(WindowInternal INTERFACE)
target_include_directories(WindowInternal INTERFACE "Source")
target_link_to_dependencies(WindowInternal
    INTERFACE Window
    INTERFACE BabylonNativeUtils
    INTERFACE JsRuntime)

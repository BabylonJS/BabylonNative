set(SOURCES
    "Include/WindowPolyfill/Babylon/WindowPolyfill.h"
    "Include/NativeWindow/Babylon/NativeWindow.h"
    "Source/NativeWindow.cpp")

add_library(WindowPolyfill ${SOURCES})

target_include_directories(WindowPolyfill
    PUBLIC "Include/WindowPolyfill"
    PRIVATE "Include/NativeWindow/Babylon")

target_link_to_dependencies(WindowPolyfill 
    PUBLIC napi
    PRIVATE base-n
    PRIVATE BabylonNativeUtils
    PRIVATE JsRuntime)

set_property(TARGET WindowPolyfill PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(WindowPolyfillImplementation INTERFACE)
target_include_directories(WindowPolyfillImplementation INTERFACE "Include/NativeWindow")
target_link_to_dependencies(WindowPolyfillImplementation
    INTERFACE WindowPolyfill
    INTERFACE BabylonNativeUtils
    INTERFACE JsRuntime)

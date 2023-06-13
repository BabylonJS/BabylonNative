set(SOURCES
    "Include/Babylon/Polyfills/Window.h"
    "Source/TimeoutDispatcher.h"
    "Source/TimeoutDispatcher.cpp"
    "Source/Window.h"
    "Source/Window.cpp")

add_library(Window ${SOURCES})
# V8 and V8Inspector have unused parameters. Unable to set warning as error.
if(NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    warnings_as_errors(Window)
endif()

target_include_directories(Window
    PUBLIC "Include")

target_link_to_dependencies(Window
    PUBLIC napi
    PRIVATE base-n
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext)

set_property(TARGET Window PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
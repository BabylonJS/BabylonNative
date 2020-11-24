set(SOURCES
    "Include/Babylon/Polyfills/Window.h"
    "Source/Window.h"
    "Source/Window.cpp")

add_library(Window ${SOURCES})
warnings_as_errors(Window)

target_include_directories(Window
    PUBLIC "Include")

if(WIN32)
    target_compile_definitions(Window PRIVATE _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

target_link_to_dependencies(Window 
    PUBLIC napi
    PRIVATE base-n
    PRIVATE JsRuntime)

set_property(TARGET Window PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

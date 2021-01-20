set(SOURCES
    "Include/Babylon/Polyfills/Window.h"
    "Source/Window.h"
    "Source/Window.cpp")

if(WIN32)
    set(SOURCES ${SOURCES} "Source/Windows/WindowPlatform.h")
endif()

add_library(Window ${SOURCES})
warnings_as_errors(Window)

target_include_directories(Window
    PUBLIC "Include")

if(WIN32)
    target_include_directories(Window 
        PRIVATE "Source/Windows")
endif()

target_link_to_dependencies(Window 
    PUBLIC napi
    PRIVATE base-n
    PRIVATE JsRuntime)

if(WIN32)
    target_link_to_dependencies(Window
        PRIVATE "shlwapi.lib")
endif()

set_property(TARGET Window PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
 
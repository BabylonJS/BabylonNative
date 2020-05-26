set(SOURCES
    "Include/Babylon/Polyfills/Script.h"
    "Source/Script.cpp"
    "Source/Script.h")

add_library(Script ${SOURCES})

target_include_directories(Script PUBLIC "Include")

target_link_to_dependencies(Script 
    PUBLIC napi 
    PRIVATE arcana)

target_link_to_dependencies(Script 
    PUBLIC napi
    PRIVATE ScriptLoader
    PRIVATE JsRuntime
    INTERFACE arcana)
    
set_property(TARGET Script PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

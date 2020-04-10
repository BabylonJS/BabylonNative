set(SOURCES 
    "Include/Babylon/CmdArgs.h"
    "Source/CmdArgs.cpp")

add_library(CmdArgs ${SOURCES})

target_include_directories(CmdArgs PUBLIC "Include")

target_link_to_dependencies(CmdArgs 
    PUBLIC JsRuntime)

set_property(TARGET CmdArgs PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

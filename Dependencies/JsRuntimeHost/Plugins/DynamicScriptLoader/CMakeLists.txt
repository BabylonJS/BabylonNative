set(SOURCES
    "Include/Babylon/Plugins/DynamicScriptLoader.h"
    "Source/DynamicScriptLoader.cpp")

add_library(DynamicScriptLoader ${SOURCES})
warnings_as_errors(DynamicScriptLoader)

target_include_directories(DynamicScriptLoader PUBLIC "Include")

target_link_libraries(DynamicScriptLoader
    PUBLIC Foundation
    PUBLIC JsRuntime
    PUBLIC napi)

set_property(TARGET DynamicScriptLoader PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

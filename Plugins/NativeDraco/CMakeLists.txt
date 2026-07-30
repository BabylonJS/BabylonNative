set(SOURCES
    "Include/Babylon/Plugins/NativeDraco.h"
    "Source/NativeDraco.cpp")

add_library(NativeDraco ${SOURCES})
warnings_as_errors(NativeDraco)

target_include_directories(NativeDraco
    PUBLIC "Include")

target_link_libraries(NativeDraco
    PUBLIC napi
    PRIVATE JsRuntimeInternal
    PRIVATE draco::draco)

set_property(TARGET NativeDraco PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Plugins/DataStream.h"
    "Source/DataStream.h"
    "Source/DataStream.cpp")

add_library(DataStream ${SOURCES})
warnings_as_errors(DataStream)

target_include_directories(DataStream PUBLIC
    "Include")

target_link_libraries(DataStream
    PUBLIC napi
    PRIVATE miniz
    PRIVATE bx
    PRIVATE JsRuntimeInternal)

set_property(TARGET DataStream PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

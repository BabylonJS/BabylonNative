set(SOURCES
    "Include/Babylon/Api.h"
    "Include/Babylon/DebugTrace.h"
    "Source/DebugTrace.cpp")

add_library(Foundation ${SOURCES})

target_include_directories(Foundation INTERFACE "Include")

set_property(TARGET Foundation PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

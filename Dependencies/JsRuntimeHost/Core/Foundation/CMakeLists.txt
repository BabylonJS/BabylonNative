set(SOURCES
    "Include/Babylon/Api.h"
    "Include/Babylon/DebugTrace.h"
    "Include/Babylon/PerfTrace.h"
    "Source/DebugTrace.cpp"
    "Source/PerfTrace.cpp")

add_library(Foundation ${SOURCES})

target_include_directories(Foundation
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

target_link_libraries(Foundation
    PUBLIC napi
    PRIVATE napi-extensions
    PRIVATE arcana)

set_property(TARGET Foundation PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

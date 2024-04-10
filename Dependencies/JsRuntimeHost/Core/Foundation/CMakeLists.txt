set(SOURCES
    "Include/Babylon/Api.h")

add_library(Foundation INTERFACE)

target_include_directories(Foundation INTERFACE "Include")

set_property(TARGET Foundation PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

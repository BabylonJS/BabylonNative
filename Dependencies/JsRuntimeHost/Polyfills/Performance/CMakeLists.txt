set(SOURCES 
    ${SOURCES}
    "Include/Babylon/Polyfills/Performance.h"
    "Source/Performance.cpp")

add_library(Performance ${SOURCES})

target_include_directories(Performance PUBLIC "Include")

target_link_libraries(Performance
    PUBLIC JsRuntime)

set_property(TARGET Performance PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

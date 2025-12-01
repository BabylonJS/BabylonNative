set(SOURCES
    "Include/Babylon/Plugins/Zip.h"
    "Source/Zip.cpp")

add_library(Zip ${SOURCES})
warnings_as_errors(Zip)

target_include_directories(Zip PUBLIC
    "Include")

target_link_libraries(Zip
    PUBLIC napi
    PRIVATE miniz
    PRIVATE bx
    PRIVATE JsRuntimeInternal)

set_property(TARGET Zip PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

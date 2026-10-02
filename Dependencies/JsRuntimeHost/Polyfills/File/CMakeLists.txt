set(SOURCES
    "Include/Babylon/Polyfills/File.h"
    "Source/File.h"
    "Source/File.cpp"
    "Source/FileReader.h"
    "Source/FileReader.cpp")

add_library(File ${SOURCES})
warnings_as_errors(File)

target_include_directories(File PUBLIC "Include")

target_link_libraries(File
    PRIVATE base-n
    PUBLIC JsRuntime)

set_property(TARGET File PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

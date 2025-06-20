set(SOURCES 
    "Include/Babylon/Polyfills/Blob.h"
    "Source/Blob.cpp"
    "Source/Blob.h")

add_library(Blob ${SOURCES})
warnings_as_errors(Blob)

target_include_directories(Blob PUBLIC "Include")

target_link_libraries(Blob
    PUBLIC JsRuntime)

set_property(TARGET Blob PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

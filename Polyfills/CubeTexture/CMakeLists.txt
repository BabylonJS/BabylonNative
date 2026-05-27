set(SOURCES
    "Include/Babylon/Polyfills/CubeTexture.h"
    "Source/CubeTexture.cpp")

add_library(CubeTexture ${SOURCES})
warnings_as_errors(CubeTexture)

target_include_directories(CubeTexture
    PUBLIC "Include")

target_link_libraries(CubeTexture
    PUBLIC napi
    PUBLIC Foundation)

set_property(TARGET CubeTexture PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

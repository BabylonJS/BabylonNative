set(SOURCES
    "Include/Babylon/Polyfills/Canvas.h"
    "Source/Canvas.cpp"
    "Source/Canvas.h"
    "Source/font/font_manager.cpp"
    "Source/font/font_manager.h"
    "Source/font/text_buffer_manager.cpp"
    "Source/font/text_buffer_manager.h"
    "Source/font/cube_atlas.cpp"
    "Source/font/cube_atlas.h"
    "Source/font/utf8.cpp"
    "Source/font/utf8.h"
    )

add_library(Canvas ${SOURCES})
warnings_as_errors(Canvas)

target_include_directories(Canvas PUBLIC "Include")

target_link_to_dependencies(Canvas PUBLIC napi
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

set_property(TARGET Canvas PROPERTY FOLDER Polyfills)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

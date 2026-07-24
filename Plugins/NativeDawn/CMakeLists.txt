set(SOURCES
    "Include/Babylon/Plugins/NativeDawn.h"
    "Source/ImageDecode.cpp"
    "Source/NativeDawn.cpp")

add_library(NativeDawn ${SOURCES})

target_include_directories(NativeDawn
    PUBLIC "Include")

target_link_libraries(NativeDawn
    PUBLIC napi
    PRIVATE JsRuntime
    PRIVATE webgpu_dawn
    PRIVATE bx
    PRIVATE bimg
    PRIVATE bimg_decode
    PRIVATE bimg_encode)

if(WIN32)
    target_compile_definitions(NativeDawn
        PRIVATE NOMINMAX)
endif()

set_property(TARGET NativeDawn PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

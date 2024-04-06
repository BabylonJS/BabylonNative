set(SOURCES
    "Include/Babylon/Plugins/NativeCapture.h"
    "Source/NativeCapture.cpp")

add_library(NativeCapture ${SOURCES})
warnings_as_errors(NativeCapture)

target_include_directories(NativeCapture PUBLIC
    "Include")

target_link_libraries(NativeCapture
    PUBLIC JsRuntimeInternal
    PRIVATE GraphicsDeviceContext)

set_property(TARGET NativeCapture PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

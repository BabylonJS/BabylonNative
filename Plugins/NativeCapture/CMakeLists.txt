set(SOURCES
    "Include/Babylon/Plugins/NativeCapture.h"
    "Source/NativeCapture.cpp")

add_library(NativeCapture ${SOURCES})
warnings_as_errors(NativeCapture)

target_include_directories(NativeCapture INTERFACE "Include")
target_include_directories(NativeCapture PRIVATE "Include/Babylon/Plugins")

target_link_to_dependencies(NativeCapture
    PUBLIC JsRuntime
    PRIVATE GraphicsDeviceContext
    PRIVATE napi_extensions)

set_property(TARGET NativeCapture PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

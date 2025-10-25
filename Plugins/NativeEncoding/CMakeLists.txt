set(SOURCES
    "Include/Babylon/Plugins/NativeEncoding.h"
    "Source/NativeEncoding.cpp")

add_library(NativeEncoding ${SOURCES})
warnings_as_errors(NativeEncoding)

target_include_directories(NativeEncoding
    PUBLIC "Include")

target_link_libraries(NativeEncoding
    PUBLIC napi
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext
    PRIVATE JsRuntimeInternal)

set_property(TARGET NativeEncoding PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Plugins/NativeZip.h"
    "Source/NativeZip.cpp")

add_library(NativeZip ${SOURCES})
warnings_as_errors(NativeZip)

target_include_directories(NativeZip PUBLIC
    "Include")

target_link_libraries(NativeZip
    PUBLIC napi
    PRIVATE minz
    PRIVATE JsRuntimeInternal)

set_property(TARGET NativeZip PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

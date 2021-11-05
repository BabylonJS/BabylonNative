set(SOURCES
    "Include/Babylon/Plugins/NativeTracing.h"
    "Source/NativeTracing.cpp")

add_library(NativeTracing ${SOURCES})
warnings_as_errors(NativeTracing)

if (APPLE)
    set_target_properties(NativeTracing PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    )
endif()

target_include_directories(NativeTracing PUBLIC "Include")

target_link_to_dependencies(NativeTracing
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE napi_extensions)

set_property(TARGET NativeTracing PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Plugins/NativePerformanceLogger.h"
    "Source/NativePerformanceLogger.cpp")

add_library(NativePerformanceLogger ${SOURCES})
warnings_as_errors(NativePerformanceLogger)

if (APPLE)
    set_target_properties(NativePerformanceLogger PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    )
endif()

target_include_directories(NativePerformanceLogger PUBLIC "Include")

target_link_to_dependencies(NativePerformanceLogger
    PUBLIC JsRuntime
    PRIVATE arcana
    PRIVATE napi_extensions)

set_property(TARGET NativePerformanceLogger PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Plugins/NativeOptimizations.h"
    "Source/NativeOptimizations.cpp")

add_library(NativeOptimizations ${SOURCES})
warnings_as_errors(NativeOptimizations)

if (APPLE)
    set_target_properties(xr PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    )
endif()

target_include_directories(NativeOptimizations PUBLIC "Include")

target_link_to_dependencies(NativeOptimizations
    PUBLIC JsRuntime)

set_property(TARGET NativeOptimizations PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

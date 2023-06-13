set(SOURCES
    "Include/Babylon/Plugins/NativeOptimizations.h"
    "Source/NativeOptimizations.cpp")

add_library(NativeOptimizations ${SOURCES})
# V8 and V8Inspector have unused parameters. Unable to set warning as error.
if(NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    warnings_as_errors(NativeOptimizations)
endif()

if(APPLE)
    set_target_properties(NativeOptimizations PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

target_include_directories(NativeOptimizations PUBLIC "Include")

target_link_to_dependencies(NativeOptimizations
    PUBLIC napi
    PRIVATE JsRuntimeInternal)

set_property(TARGET NativeOptimizations PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

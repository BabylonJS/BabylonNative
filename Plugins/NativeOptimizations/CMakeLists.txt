set(SOURCES
    "Include/Babylon/Plugins/NativeOptimizations.h"
    "Source/NativeOptimizations.cpp")

add_library(NativeOptimizations ${SOURCES})
warnings_as_errors(NativeOptimizations)

if(APPLE)
    enable_objc_arc(NativeOptimizations)
endif()

target_include_directories(NativeOptimizations PUBLIC "Include")

target_link_libraries(NativeOptimizations
    PUBLIC napi
    PRIVATE JsRuntimeInternal)

set_property(TARGET NativeOptimizations PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

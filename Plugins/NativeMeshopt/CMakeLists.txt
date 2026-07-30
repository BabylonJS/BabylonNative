set(SOURCES
    "Include/Babylon/Plugins/NativeMeshopt.h"
    "Source/NativeMeshopt.cpp")

add_library(NativeMeshopt ${SOURCES})
warnings_as_errors(NativeMeshopt)

target_include_directories(NativeMeshopt
    PUBLIC "Include")

target_link_libraries(NativeMeshopt
    PUBLIC napi
    PRIVATE JsRuntimeInternal
    PRIVATE meshoptimizer)

set_property(TARGET NativeMeshopt PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

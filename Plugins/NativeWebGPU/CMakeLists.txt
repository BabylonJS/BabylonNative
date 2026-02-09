set(SOURCES
    "Include/Babylon/Plugins/NativeWebGPU.h"
    "Source/NativeWebGPU.cpp")

add_library(NativeWebGPU ${SOURCES})
warnings_as_errors(NativeWebGPU)

target_include_directories(NativeWebGPU
    PUBLIC "Include")

target_link_libraries(NativeWebGPU
    PUBLIC napi
    PRIVATE JsRuntimeInternal)

if(TARGET babylon_graphics_backend)
    target_link_libraries(NativeWebGPU PRIVATE babylon_graphics_backend)
endif()

set_property(TARGET NativeWebGPU PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Plugins/NativeXr.h"
    "Source/NativeXr.cpp")

add_library(NativeXr ${SOURCES})
warnings_as_errors(NativeXr)

target_include_directories(NativeXr
    PRIVATE "Include/Babylon/Plugins"
    INTERFACE "Include")

target_link_to_dependencies(NativeXr
    PUBLIC JsRuntime
    PRIVATE NativeEngineInternal
    PRIVATE xr
    PRIVATE arcana)

set_property(TARGET NativeXr PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(WINDOWS_STORE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

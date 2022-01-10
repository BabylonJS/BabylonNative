set(SOURCES
    "Include/Babylon/Graphics/ExternalResourceWrapper.h"
    "Source/ExternalResourceWrapper.cpp")

add_library(GraphicsExternalResourceWrapper ${SOURCES})
warnings_as_errors(GraphicsExternalResourceWrapper)

target_include_directories(GraphicsExternalResourceWrapper INTERFACE "Include")
target_include_directories(GraphicsExternalResourceWrapper PRIVATE "Include/Babylon/Graphics")

target_link_to_dependencies(GraphicsExternalResourceWrapper
    PUBLIC JsRuntime
    PRIVATE GraphicsContext
    PRIVATE napi_extensions)

set_property(TARGET GraphicsExternalResourceWrapper PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

set(SOURCES
    "Include/Babylon/Graphics/ExternalResource.h"
    "Source/ExternalResource.cpp")

add_library(GraphicsExternalResource ${SOURCES})
warnings_as_errors(GraphicsExternalResource)

target_include_directories(GraphicsExternalResource INTERFACE "Include")
target_include_directories(GraphicsExternalResource PRIVATE "Include/Babylon/Graphics")

target_link_to_dependencies(GraphicsExternalResource
    PUBLIC JsRuntime
    PRIVATE GraphicsContext
    PRIVATE napi_extensions)

set_property(TARGET GraphicsExternalResource PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

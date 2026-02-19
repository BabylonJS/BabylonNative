set(SOURCES
    "Include/Babylon/Plugins/ExternalTexture.h"
    "Source/ExternalTexture_${GRAPHICS_API}.cpp"
    "Source/ExternalTexture_Base.h"
    "Source/ExternalTexture_Shared.h")

add_library(ExternalTexture ${SOURCES})
warnings_as_errors(ExternalTexture)

target_include_directories(ExternalTexture
    PUBLIC "Include")

target_link_libraries(ExternalTexture
    PUBLIC napi
    PUBLIC GraphicsDevice
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext)

target_compile_definitions(ExternalTexture
    PRIVATE NOMINMAX)

set_property(TARGET ExternalTexture PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

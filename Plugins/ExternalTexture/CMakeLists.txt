set(SOURCES
    "Include/Babylon/Plugins/ExternalTexture.h"
    "Source/ExternalTexture_${GRAPHICS_API}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}"
    "Source/ExternalTexture_Base.h"
    "Source/ExternalTexture_Shared.h")

add_library(ExternalTexture ${SOURCES})
warnings_as_errors(ExternalTexture)

if (APPLE)
    set_target_properties(ExternalTexture PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
endif()

target_include_directories(ExternalTexture
    PUBLIC "Include")

target_link_to_dependencies(ExternalTexture
    PUBLIC napi
    PUBLIC GraphicsDevice
    PRIVATE napi_extensions
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDeviceContext)

target_compile_definitions(ExternalTexture
    PRIVATE NOMINMAX)

set_property(TARGET ExternalTexture PROPERTY FOLDER Plugins)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

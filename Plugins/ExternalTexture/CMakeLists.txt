if(WINDOWS_STORE)
    if(BABYLON_NATIVE_USE_SWAPCHAINPANEL)
        set(PLATFORM_NAME ${BABYLON_NATIVE_PLATFORM}_SwapChainPanel)
        set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM}/SwapChainPanel)
    else()
        set(PLATFORM_NAME ${BABYLON_NATIVE_PLATFORM}_CoreWindow)
        set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM}/CoreWindow)
    endif()
else()
    set(PLATFORM_NAME ${BABYLON_NATIVE_PLATFORM})
    set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM})
endif()

set(SOURCES
    "Include/Shared/Babylon/Plugins/ExternalTexture/ExternalTexture.h"
    "Source/ExternalTexture_${GRAPHICS_API}.cpp"
    "Source/ExternalTexture.cpp")

add_library(ExternalTexture ${SOURCES})
warnings_as_errors(ExternalTexture)

target_include_directories(ExternalTexture
    PUBLIC "Include/Shared"
    PRIVATE "Source")

if(WIN32 AND NOT WINDOWS_STORE)
    target_link_to_dependencies(ExternalTexture
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_to_dependencies(ExternalTexture
        PRIVATE android
        PRIVATE AndroidExtensions)
endif()

target_link_to_dependencies(ExternalTexture
    PRIVATE napi_extensions
    PRIVATE JsRuntime
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext
    PRIVATE Graphics
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

target_compile_definitions(ExternalTexture
    PRIVATE NOMINMAX
    PRIVATE ${GRAPHICS_API_UPPER})

set_property(TARGET ExternalTexture PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(APPLE)
    # compile as objc++
    target_compile_options(ExternalTexture PUBLIC "SHELL:-x objective-c++")
endif()
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
    "Include/Shared/Babylon/GraphicsInterops/ExternalTexture.h"
    "Source/Platform/${PLATFORM_PATH}/ExternalTexturePlatform.cpp"
    "Source/ExternalTexture.cpp"
    "Source/ExternalTextureImpl.h"
    "Source/ExternalTextureImpl.cpp")

add_library(GraphicsInterops ${SOURCES})
warnings_as_errors(GraphicsInterops)

target_include_directories(GraphicsInterops
    PUBLIC "Include/Shared"
    PRIVATE "Source"
    PRIVATE "Include/Platform/${PLATFORM_PATH}")

if(WIN32 AND NOT WINDOWS_STORE)
    target_link_to_dependencies(GraphicsInterops
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_to_dependencies(GraphicsInterops
        PRIVATE android
        PRIVATE AndroidExtensions)
endif()

target_link_to_dependencies(GraphicsInterops
    PRIVATE napi_extensions
    PRIVATE JsRuntime
    PRIVATE JsRuntimeInternal
    PRIVATE GraphicsDevice
    PRIVATE GraphicsDeviceContext
    PRIVATE Graphics
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

target_compile_definitions(GraphicsInterops
    PRIVATE NOMINMAX
    PRIVATE ${GRAPHICS_API_UPPER})

set_property(TARGET GraphicsInterops PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(APPLE)
    # compile as objc++
    target_compile_options(GraphicsInterops PUBLIC "SHELL:-x objective-c++")
endif()
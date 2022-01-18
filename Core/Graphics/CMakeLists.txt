if(WINDOWS_STORE)
    if(BABYLON_NATIVE_WINDOWS_STORE_USE_SWAPCHAINPANEL)
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
    "Include/Platform/${PLATFORM_PATH}/Babylon/GraphicsPlatform.h"
    "Include/RendererType/${GRAPHICS_API}/Babylon/GraphicsRendererType.h"
    "Include/Shared/Babylon/Graphics.h"
    "InternalInclude/BgfxCallback.h"
    "InternalInclude/continuation_scheduler.h"
    "InternalInclude/FrameBuffer.h"
    "InternalInclude/GraphicsContext.h"
    "InternalInclude/SafeTimespanGuarantor.h"
    "InternalInclude/Texture.h"
    "Source/BgfxCallback.cpp"
    "Source/FrameBuffer.cpp"
    "Source/Graphics.cpp"
    "Source/GraphicsContext.cpp"
    "Source/GraphicsImpl.cpp"
    "Source/GraphicsImpl.h"
    "Source/GraphicsImpl_${PLATFORM_NAME}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}"
    "Source/GraphicsImpl_${GRAPHICS_API}.cpp"
    "Source/SafeTimespanGuarantor.cpp"
    "Source/Texture.cpp")

add_library(GraphicsImplementation ${SOURCES})
warnings_as_errors(GraphicsImplementation)

target_include_directories(GraphicsImplementation
    PRIVATE "Include/Shared"
    PRIVATE "Include/Platform/${PLATFORM_PATH}"
    PRIVATE "Include/RendererType/${GRAPHICS_API}"
    PRIVATE "InternalInclude/")

if(WIN32 AND NOT WINDOWS_STORE)
    target_link_to_dependencies(GraphicsImplementation
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_to_dependencies(GraphicsImplementation
        PRIVATE android
        PRIVATE AndroidExtensions)
endif()

target_link_to_dependencies(GraphicsImplementation
    PRIVATE napi_extensions
    PRIVATE JsRuntime
    PRIVATE JsRuntimeInternal
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

string(TOUPPER ${GRAPHICS_API} GRAPHICS_API_UPPER)
target_compile_definitions(GraphicsImplementation
    PRIVATE NOMINMAX
    PRIVATE ${GRAPHICS_API_UPPER})

set_property(TARGET GraphicsImplementation PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(APPLE)
    # compile as objc++
    target_compile_options(GraphicsImplementation PUBLIC "SHELL:-x objective-c++")
endif()

add_library(Graphics INTERFACE)
target_include_directories(Graphics
    INTERFACE "Include/Shared"
    INTERFACE "Include/Platform/${PLATFORM_PATH}"
    INTERFACE "Include/RendererType/${GRAPHICS_API}")
target_link_to_dependencies(Graphics
    INTERFACE GraphicsImplementation
    INTERFACE JsRuntime)

add_library(GraphicsContext INTERFACE)
target_include_directories(GraphicsContext
    INTERFACE "InternalInclude"
    INTERFACE "InternalInclude/${BABYLON_NATIVE_PLATFORM}")
target_link_to_dependencies(GraphicsContext
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx)

target_compile_definitions(GraphicsContext
    INTERFACE NOMINMAX)

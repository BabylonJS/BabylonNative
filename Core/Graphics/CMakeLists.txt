set(SOURCES
    "Include/Platform/${BABYLON_NATIVE_PLATFORM}/Babylon/Graphics/Platform.h"
    "Include/RendererType/${GRAPHICS_API}/Babylon/Graphics/RendererType.h"
    "Include/Shared/Babylon/Graphics/Device.h"
    "InternalInclude/Babylon/Graphics/BgfxCallback.h"
    "InternalInclude/Babylon/Graphics/continuation_scheduler.h"
    "InternalInclude/Babylon/Graphics/FrameBuffer.h"
    "InternalInclude/Babylon/Graphics/DeviceContext.h"
    "InternalInclude/Babylon/Graphics/SafeTimespanGuarantor.h"
    "InternalInclude/Babylon/Graphics/Texture.h"
    "Source/BgfxCallback.cpp"
    "Source/FrameBuffer.cpp"
    "Source/Device.cpp"
    "Source/DeviceContext.cpp"
    "Source/DeviceImpl.cpp"
    "Source/DeviceImpl.h"
    "Source/DeviceImpl_${BABYLON_NATIVE_PLATFORM}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}"
    "Source/DeviceImpl_${GRAPHICS_API}.cpp"
    "Source/SafeTimespanGuarantor.cpp"
    "Source/Texture.cpp")

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
    PRIVATE "Include/Shared"
    PRIVATE "Include/Platform/${BABYLON_NATIVE_PLATFORM}"
    PRIVATE "Include/RendererType/${GRAPHICS_API}"
    PRIVATE "InternalInclude")

if(WINDOWS_STORE)
    target_link_libraries(Graphics
        PRIVATE WindowsAppSDK)
elseif(WIN32)
    target_link_libraries(Graphics
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_libraries(Graphics
        PRIVATE android
        PRIVATE AndroidExtensions)
endif()

target_link_libraries(Graphics
    PRIVATE JsRuntimeInternal
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bimg_encode
    PRIVATE bimg_decode
    PRIVATE minz
    PRIVATE bx)

target_compile_definitions(Graphics
    PRIVATE NOMINMAX)

# TODO: remove this once the #define in GraphicsImpl is removed
target_compile_definitions(Graphics
    PRIVATE $<UPPER_CASE:${GRAPHICS_API}>)

set_property(TARGET Graphics PROPERTY FOLDER Core)
set_property(TARGET Graphics PROPERTY UNITY_BUILD false)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

if(APPLE)
    # compile as objc++
    target_compile_options(Graphics PUBLIC "SHELL:-x objective-c++")
endif()

add_library(GraphicsDevice INTERFACE)
target_include_directories(GraphicsDevice
    INTERFACE "Include/Shared"
    INTERFACE "Include/Platform/${BABYLON_NATIVE_PLATFORM}"
    INTERFACE "Include/RendererType/${GRAPHICS_API}")
target_link_libraries(GraphicsDevice
    INTERFACE Graphics
    INTERFACE JsRuntime)

add_library(GraphicsDeviceContext INTERFACE)
target_include_directories(GraphicsDeviceContext
    INTERFACE "InternalInclude"
    INTERFACE "InternalInclude/${BABYLON_NATIVE_PLATFORM}")
target_link_libraries(GraphicsDeviceContext
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx)

target_compile_definitions(GraphicsDeviceContext
    INTERFACE NOMINMAX)

if(BABYLON_NATIVE_CHECK_THREAD_AFFINITY)
    target_compile_definitions(Graphics PRIVATE BABYLON_NATIVE_CHECK_THREAD_AFFINITY=1)
endif()

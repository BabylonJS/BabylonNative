set(SOURCES
    "Include/Platform/${BABYLON_NATIVE_PLATFORM}/Babylon/Graphics/Platform.h"
    "Include/RendererType/${GRAPHICS_API}/Babylon/Graphics/RendererType.h"
    "Include/Shared/Babylon/Graphics/Device.h"
    "InternalInclude/Babylon/Graphics/BgfxCallback.h"
    "InternalInclude/Babylon/Graphics/continuation_scheduler.h"
    "InternalInclude/Babylon/Graphics/DeviceContext.h"
    "InternalInclude/Babylon/Graphics/DeviceQueries.h"
    "InternalInclude/Babylon/Graphics/FrameBuffer.h"
    "InternalInclude/Babylon/Graphics/Texture.h"
    "Source/BgfxCallback.cpp"
    "Source/FrameBuffer.cpp"
    "Source/Device.cpp"
    "Source/DeviceContext.cpp"
    "Source/DeviceImpl.cpp"
    "Source/DeviceImpl.h"
    "Source/DeviceImpl_${BABYLON_NATIVE_PLATFORM}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}"
    "Source/DeviceImpl_${GRAPHICS_API}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}"
    "Source/Texture.cpp")

if(GRAPHICS_API STREQUAL "OpenGL")
    list(APPEND SOURCES
        "Include/RendererType/OpenGL/Babylon/Graphics/GL/SharedPtr.h"
        "Include/RendererType/OpenGL/Babylon/Graphics/GL/Texture.h")
endif()

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
    PRIVATE "Include/Shared"
    PRIVATE "Include/Platform/${BABYLON_NATIVE_PLATFORM}"
    PRIVATE "Include/RendererType/${GRAPHICS_API}"
    PRIVATE "InternalInclude")

target_compile_definitions(Graphics
    PRIVATE NOMINMAX)

# TODO: remove this once the #define in GraphicsImpl is removed
target_compile_definitions(Graphics
    PRIVATE $<UPPER_CASE:${GRAPHICS_API}>)

target_link_libraries(Graphics
    PRIVATE JsRuntimeInternal
    PRIVATE bgfx
    PRIVATE minz
    PRIVATE bx)

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
elseif(APPLE)
    target_link_libraries(Graphics
        PRIVATE metal-cpp)
    if(IOS)
        # bgfx's metal-cpp (compiled as C++ against the iOS 26 SDK) emits direct link-time
        # externs for the MTLIOErrorDomain / MTLTensorDomain constants. The iPhoneSimulator
        # Metal stub in Xcode 26.x does not export them (the device stub does), so simulator
        # links fail with undefined symbols. Allow these two symbols to be resolved by dynamic
        # lookup at runtime; this is harmless on device, where they resolve normally. Propagated
        # to consumers (the final app link) via INTERFACE.
        target_link_options(Graphics INTERFACE
            "LINKER:-U,_MTLIOErrorDomain"
            "LINKER:-U,_MTLTensorDomain")
    endif()
endif()

set_property(TARGET Graphics PROPERTY FOLDER Core)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# --------------
# GraphicsDevice
# --------------

add_library(GraphicsDevice INTERFACE)

target_include_directories(GraphicsDevice
    INTERFACE "Include/Shared"
    INTERFACE "Include/Platform/${BABYLON_NATIVE_PLATFORM}"
    INTERFACE "Include/RendererType/${GRAPHICS_API}")

target_link_libraries(GraphicsDevice
    INTERFACE Graphics
    INTERFACE JsRuntime)

if(APPLE)
    target_link_libraries(GraphicsDevice
        INTERFACE metal-cpp)
endif()

if(ANGLE_GLESV2_IMPORT_LIB)
    # Consumers of the OpenGL RendererType headers above may call GL ES directly.
    target_link_libraries(GraphicsDevice
        INTERFACE "${ANGLE_GLESV2_IMPORT_LIB}")

    # Windows has no GL ES SDK, and the browser ships ANGLE's DLLs without headers, so those
    # consumers also need somewhere to find <GLES3/gl3.h>. bgfx vendors the Khronos headers
    # but keeps the directory PRIVATE, so name it here rather than relying on that target.
    target_include_directories(GraphicsDevice
        INTERFACE "${BGFX_KHRONOS_INCLUDE_DIR}")
endif()

# ---------------------
# GraphicsDeviceContext
# ---------------------

add_library(GraphicsDeviceContext INTERFACE)

target_include_directories(GraphicsDeviceContext
    INTERFACE "InternalInclude"
    INTERFACE "InternalInclude/${BABYLON_NATIVE_PLATFORM}")

target_link_libraries(GraphicsDeviceContext
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bx)

if(APPLE)
    target_link_libraries(GraphicsDeviceContext
        INTERFACE metal-cpp)
endif()

target_compile_definitions(GraphicsDeviceContext
    INTERFACE NOMINMAX)

if(BABYLON_NATIVE_CHECK_THREAD_AFFINITY)
    target_compile_definitions(Graphics PRIVATE BABYLON_NATIVE_CHECK_THREAD_AFFINITY=1)
endif()

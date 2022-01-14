cmake_dependent_option(BABYLON_NATIVE_WINDOWS_STORE_USE_SWAPCHAINPANEL "Use swap chain panel." OFF WINDOWS_STORE OFF)

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
    "Source/continuation_scheduler.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/FrameBuffer.cpp"
    "Source/FrameBuffer.h"
    "Source/Graphics.cpp"
    "Source/GraphicsImpl.cpp"
    "Source/GraphicsImpl.h"
    "Source/GraphicsImpl_${PLATFORM_NAME}.${BABYLON_NATIVE_PLATFORM_IMPL_EXT}"
    "Source/GraphicsImpl_${GRAPHICS_API}.cpp"
    "Source/SafeTimespanGuarantor.cpp"
    "Source/SafeTimespanGuarantor.h")

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
    PUBLIC "Include/Shared"
    PUBLIC "Include/Platform/${PLATFORM_PATH}"
    PUBLIC "Include/RendererType/${GRAPHICS_API}")

if(WIN32 AND NOT WINDOWS_STORE)
    target_link_to_dependencies(Graphics
        PRIVATE "shlwapi.lib")
elseif(ANDROID)
    target_link_to_dependencies(Graphics
        PRIVATE android
        PRIVATE AndroidExtensions)
endif()

target_link_to_dependencies(Graphics
    PUBLIC JsRuntime
    PRIVATE JsRuntimeInternal
    PRIVATE bgfx
    PRIVATE bimg
    PRIVATE bx)

string(TOUPPER ${GRAPHICS_API} GRAPHICS_API_UPPER)
target_compile_definitions(Graphics
    PRIVATE NOMINMAX
    PRIVATE ${GRAPHICS_API_UPPER})

set_property(TARGET Graphics PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

add_library(GraphicsInternal INTERFACE)
target_include_directories(GraphicsInternal
    INTERFACE "Source"
    INTERFACE "Source/${BABYLON_NATIVE_PLATFORM}")
target_link_to_dependencies(GraphicsInternal
    INTERFACE Graphics
    INTERFACE JsRuntimeInternal
    INTERFACE arcana
    INTERFACE bgfx
    INTERFACE bimg
    INTERFACE bx)

target_compile_definitions(GraphicsInternal
    INTERFACE NOMINMAX)

if(APPLE)
    # compile as objc++
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")
endif()

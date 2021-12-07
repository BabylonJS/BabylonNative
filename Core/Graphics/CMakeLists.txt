set(SOURCES
    "Include/Shared/Babylon/Graphics.h"
    "Source/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatformImpl.h"
    "Source/continuation_scheduler.h"
    "Source/BgfxCallback.cpp"
    "Source/BgfxCallback.h"
    "Source/FrameBuffer.cpp"
    "Source/FrameBuffer.h"
    "Source/Graphics.cpp"
    "Source/GraphicsImpl.cpp"
    "Source/GraphicsImpl.h"
    "Source/SafeTimespanGuarantor.cpp"
    "Source/SafeTimespanGuarantor.h")

if (WINDOWS_STORE)
    if (BABYLON_NATIVE_WINDOWS_STORE_USE_SWAPCHAINPANEL)
        set(SOURCES
            ${SOURCES}
            "Include/${BABYLON_NATIVE_PLATFORM}/SwapChainPanel/Babylon/GraphicsPlatform.h"
            "Source/${BABYLON_NATIVE_PLATFORM}/SwapChainPanel/GraphicsPlatform.cpp")
    else()
        set(SOURCES
            ${SOURCES}
            "Include/${BABYLON_NATIVE_PLATFORM}/CoreWindow/Babylon/GraphicsPlatform.h"
            "Source/${BABYLON_NATIVE_PLATFORM}/CoreWindow/GraphicsPlatform.cpp")
    endif()
elseif(APPLE)
    if(IOS)
        set(SOURCES
            ${SOURCES}
            "Include/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatform.h"
            "Source/${BABYLON_NATIVE_PLATFORM}/iOS/GraphicsPlatform.mm")
    else()
        set(SOURCES
            ${SOURCES}
            "Include/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatform.h"
            "Source/${BABYLON_NATIVE_PLATFORM}/macOS/GraphicsPlatform.mm")
    endif()
else()
    set(SOURCES
        ${SOURCES}
        "Include/${BABYLON_NATIVE_PLATFORM}/Babylon/GraphicsPlatform.h"
        "Source/${BABYLON_NATIVE_PLATFORM}/GraphicsPlatform.cpp")
endif()

add_library(Graphics ${SOURCES})
warnings_as_errors(Graphics)

target_include_directories(Graphics
            PUBLIC "Include/Shared"
            PRIVATE "Source"
            PRIVATE "Source/${BABYLON_NATIVE_PLATFORM}")

if(WINDOWS_STORE)
    if(BABYLON_NATIVE_WINDOWS_STORE_USE_SWAPCHAINPANEL)
        target_include_directories(Graphics
            PUBLIC "Include/${BABYLON_NATIVE_PLATFORM}/SwapChainPanel")
    else()
        target_include_directories(Graphics
            PUBLIC "Include/${BABYLON_NATIVE_PLATFORM}/CoreWindow")
    endif()
else()
    target_include_directories(Graphics
            PUBLIC "Include/${BABYLON_NATIVE_PLATFORM}")
endif()


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

# compile as objc++
if (APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++")
endif()

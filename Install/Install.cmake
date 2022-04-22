include(GNUInstallDirs)

# Dependencies

## bgfx
install(TARGETS astc astc-codec edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr bgfx bimg bx)

## glslang
install(TARGETS GenericCodeGen glslang MachineIndependent OGLCompiler OSDependent SPIRV)

## SPIRV-Cross
install(TARGETS spirv-cross-core spirv-cross-glsl)
if(TARGET spirv-cross-msl)
    install(TARGETS spirv-cross-msl)
endif()
if(TARGET spirv-cross-hlsl)
    install(TARGETS spirv-cross-hlsl)
endif()

## XR
if(TARGET openxr_loader)
    install(TARGETS openxr_loader)
endif()

## napi
install(DIRECTORY Dependencies/napi/napi-direct/include/napi TYPE INCLUDE)

## UrlLib
install(TARGETS UrlLib)

# Core

install(TARGETS JsRuntime)
install(DIRECTORY Core/JsRuntime/Include/Babylon TYPE INCLUDE)

install(TARGETS Graphics)
if(WINDOWS_STORE)
    if(BABYLON_NATIVE_USE_SWAPCHAINPANEL)
        set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM}/SwapChainPanel)
    else()
        set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM}/CoreWindow)
    endif()
else()
    set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM})
endif()
install(DIRECTORY Core/Graphics/Include/Platform/${PLATFORM_PATH}/Babylon TYPE INCLUDE)
install(DIRECTORY Core/Graphics/Include/RendererType/${GRAPHICS_API}/Babylon TYPE INCLUDE)
install(DIRECTORY Core/Graphics/Include/Shared/Babylon TYPE INCLUDE)

if(TARGET AppRuntime)
    install(TARGETS AppRuntime)
    install(DIRECTORY Core/AppRuntime/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET ScriptLoader)
    install(TARGETS ScriptLoader)
    install(DIRECTORY Core/ScriptLoader/Include/Babylon TYPE INCLUDE)
endif()

# Plugins

if(TARGET ChromeDevTools)
    install(TARGETS ChromeDevTools)
    install(DIRECTORY Plugins/ChromeDevTools/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET ExternalTexture)
    install(TARGETS ExternalTexture)
    install(DIRECTORY Plugins/ExternalTexture/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeCamera)
    install(TARGETS NativeCamera)
    install(DIRECTORY Plugins/NativeCamera/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeCapture)
    install(TARGETS NativeCapture)
    install(DIRECTORY Plugins/NativeCapture/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeEngine)
    install(TARGETS NativeEngine)
    install(DIRECTORY Plugins/NativeEngine/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeInput)
    install(TARGETS NativeInput)
    install(DIRECTORY Plugins/NativeInput/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeOptimizations)
    install(TARGETS NativeOptimizations)
    install(DIRECTORY Plugins/NativeOptimizations/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeTracing)
    install(TARGETS NativeTracing)
    install(DIRECTORY Plugins/NativeTracing/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET NativeXr)
    install(TARGETS NativeXr)
    install(DIRECTORY Plugins/NativeXr/Include/Babylon TYPE INCLUDE)
endif()

# Polyfills

if(TARGET Canvas)
    install(TARGETS Canvas)
    install(DIRECTORY Polyfills/Canvas/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET Console)
    install(TARGETS Console)
    install(DIRECTORY Polyfills/Console/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET Window)
    install(TARGETS Window)
    install(DIRECTORY Polyfills/Window/Include/Babylon TYPE INCLUDE)
endif()

if(TARGET XMLHttpRequest)
    install(TARGETS XMLHttpRequest)
    install(DIRECTORY Polyfills/XMLHttpRequest/Include/Babylon TYPE INCLUDE)
endif()
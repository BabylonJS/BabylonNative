include(GNUInstallDirs)

function(install_targets)
    install(TARGETS ${ARGN})

    # Install the pdb files if they exist
    foreach(target IN LISTS ARGN)
        get_target_property(target_type ${target} TYPE)
        if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
            install(FILES $<TARGET_FILE_DIR:${target}>/${target}.pdb DESTINATION lib OPTIONAL)
        endif()
    endforeach()
endfunction()

function(install_include)
    install(DIRECTORY ${ARGV0} TYPE INCLUDE)
endfunction()

# ----------------
# Dependencies
# ----------------

## arcana.cpp
install_targets(arcana)

## bgfx
install_targets(astc astc-codec edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr bgfx bimg bx)

## glslang
install_targets(GenericCodeGen glslang MachineIndependent OGLCompiler OSDependent SPIRV)

## SPIRV-Cross
install_targets(spirv-cross-core spirv-cross-glsl)
if(TARGET spirv-cross-msl)
    install_targets(spirv-cross-msl)
endif()
if(TARGET spirv-cross-hlsl)
    install_targets(spirv-cross-hlsl)
endif()

## XR
if(TARGET openxr_loader)
    install_targets(openxr_loader)
endif()

## napi
install_include(Dependencies/napi/napi-direct/include/napi)
install_targets(napi)

## UrlLib
install_targets(UrlLib)

# ----------------
# Core
# ----------------

install_targets(JsRuntime)
install_include(Core/JsRuntime/Include/Babylon)

install_targets(Graphics)
if(WINDOWS_STORE)
    if(BABYLON_NATIVE_USE_SWAPCHAINPANEL)
        set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM}/SwapChainPanel)
    else()
        set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM}/CoreWindow)
    endif()
else()
    set(PLATFORM_PATH ${BABYLON_NATIVE_PLATFORM})
endif()
install_include(Core/Graphics/Include/Platform/${PLATFORM_PATH}/Babylon)
install_include(Core/Graphics/Include/RendererType/${GRAPHICS_API}/Babylon)
install_include(Core/Graphics/Include/Shared/Babylon)

if(TARGET AppRuntime)
    install_targets(AppRuntime)
    install_include(Core/AppRuntime/Include/Babylon)
endif()

if(TARGET ScriptLoader)
    install_targets(ScriptLoader)
    install_include(Core/ScriptLoader/Include/Babylon)
endif()

# ----------------
# Plugins
# ----------------

if(TARGET ChromeDevTools)
    install_targets(ChromeDevTools)
    install_include(Plugins/ChromeDevTools/Include/Babylon)
endif()

if(TARGET ExternalTexture)
    install_targets(ExternalTexture)
    install_include(Plugins/ExternalTexture/Include/Babylon)
endif()

if(TARGET NativeCamera)
    install_targets(NativeCamera)
    install_include(Plugins/NativeCamera/Include/Babylon)
endif()

if(TARGET NativeCapture)
    install_targets(NativeCapture)
    install_include(Plugins/NativeCapture/Include/Babylon)
endif()

if(TARGET NativeEngine)
    install_targets(NativeEngine)
    install_include(Plugins/NativeEngine/Include/Babylon)
endif()

if(TARGET NativeInput)
    install_targets(NativeInput)
    install_include(Plugins/NativeInput/Include/Babylon)
endif()

if(TARGET NativeOptimizations)
    install_targets(NativeOptimizations)
    install_include(Plugins/NativeOptimizations/Include/Babylon)
endif()

if(TARGET NativeTracing)
    install_targets(NativeTracing)
    install_include(Plugins/NativeTracing/Include/Babylon)
endif()

if(TARGET NativeXr)
    install_targets(NativeXr)
    install_include(Plugins/NativeXr/Include/Babylon)
endif()

# ----------------
# Polyfills
# ----------------

if(TARGET Canvas)
    install_targets(Canvas)
    install_include(Polyfills/Canvas/Include/Babylon)
endif()

if(TARGET Console)
    install_targets(Console)
    install_include(Polyfills/Console/Include/Babylon)
endif()

if(TARGET Window)
    install_targets(Window)
    install_include(Polyfills/Window/Include/Babylon)
endif()

if(TARGET XMLHttpRequest)
    install_targets(XMLHttpRequest)
    install_include(Polyfills/XMLHttpRequest/Include/Babylon)
endif()
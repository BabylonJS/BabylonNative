include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "./install/" CACHE PATH "..." FORCE)
endif()

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
install_targets(astc-encoder edtaa3 etc1 etc2 iqa nvtt pvrtc squish tinyexr bgfx bimg bx)

## glslang
install_targets(GenericCodeGen glslang MachineIndependent OGLCompiler OSDependent SPIRV glslang-default-resource-limits)

## SPIRV-Cross
install_targets(spirv-cross-core)
if(TARGET spirv-cross-glsl)
    install_targets(spirv-cross-glsl)
endif()
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

## UrlLib
install_targets(UrlLib)

# ----------------
# Core
# ----------------
install_targets(JsRuntime)
install_include(${jsruntimehost_SOURCE_DIR}/Core/JsRuntime/Include/Babylon)

install_targets(Graphics)
install_include(Core/Graphics/Include/Platform/${BABYLON_NATIVE_PLATFORM}/Babylon)
install_include(Core/Graphics/Include/RendererType/${GRAPHICS_API}/Babylon)
install_include(Core/Graphics/Include/Shared/Babylon)

if(TARGET AppRuntime)
    install_targets(AppRuntime)
    install_include(${jsruntimehost_SOURCE_DIR}/Core/AppRuntime/Include/Babylon)
endif()

if(TARGET ScriptLoader)
    install_targets(ScriptLoader)
    install_include(${jsruntimehost_SOURCE_DIR}/Core/ScriptLoader/Include/Babylon)
endif()

install_targets(napi)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    install_targets(v8inspector)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    install_include(${jsruntimehost_SOURCE_DIR}/Core/Node-API-JSI/Include/napi)
    set(V8JSI_VERSION "0.64.33")
    set(NUGET_PATH "${CMAKE_BINARY_DIR}/NuGet")
    if (WINDOWS_STORE)
        set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.UWP.${V8JSI_VERSION}")
    else()
        set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
    endif()
    install_include(${V8JSI_PACKAGE_PATH}/build/native/jsi/jsi)
else()
    install_include(${jsruntimehost_SOURCE_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}/napi)
    install_include(${jsruntimehost_SOURCE_DIR}/Core/Node-API/Include/Shared/napi)
endif()

# ----------------
# Plugins
# ----------------
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
    install_include(${jsruntimehost_SOURCE_DIR}/Polyfills/Console/Include/Babylon)
endif()

if(TARGET Window)
    install_targets(Window)
    install_include(Polyfills/Window/Include/Babylon)
endif()

if(TARGET XMLHttpRequest)
    install_targets(XMLHttpRequest)
    install_include(${jsruntimehost_SOURCE_DIR}/Polyfills/XMLHttpRequest/Include/Babylon)
endif()
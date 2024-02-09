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
            install(FILES "$<TARGET_FILE_DIR:${target}>/${target}.pdb" DESTINATION lib OPTIONAL)
        endif()
    endforeach()
endfunction()

function(install_include_for_target)
    get_target_property(target_interface_include_directories ${ARGV0} INTERFACE_INCLUDE_DIRECTORIES)
    foreach(include_directory IN LISTS target_interface_include_directories)
        install(DIRECTORY "${include_directory}/${ARGV1}" TYPE INCLUDE)
    endforeach()
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
install_include_for_target(JsRuntime Babylon)

 # Note libs are in the `Graphics` target but includes are in `GraphicsDevice` target
install_targets(Graphics)
install_include_for_target(GraphicsDevice Babylon)

if(TARGET AppRuntime)
    install_targets(AppRuntime)
    install_include_for_target(AppRuntime Babylon)
endif()

if(TARGET ScriptLoader)
    install_targets(ScriptLoader)
    install_include_for_target(ScriptLoader Babylon)
endif()

install_targets(napi)
install_include_for_target(napi napi)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    install_targets(v8inspector)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(V8JSI_VERSION "0.64.33")
    set(NUGET_PATH "${CMAKE_BINARY_DIR}/NuGet")
    if (WINDOWS_STORE)
        set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.UWP.${V8JSI_VERSION}")
    else()
        set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
    endif()
    #TODO
    #install_include(${V8JSI_PACKAGE_PATH}/build/native/jsi/jsi)
endif()

# ----------------
# Plugins
# ----------------
if(TARGET ExternalTexture)
    install_targets(ExternalTexture)
    install_include_for_target(ExternalTexture Babylon)
endif()

if(TARGET NativeCamera)
    install_targets(NativeCamera)
    install_include_for_target(NativeCamera Babylon)
endif()

if(TARGET NativeCapture)
    install_targets(NativeCapture)
    install_include_for_target(NativeCapture Babylon)
endif()

if(TARGET NativeEngine)
    install_targets(NativeEngine)
    install_include_for_target(NativeEngine Babylon)
endif()

if(TARGET NativeInput)
    install_targets(NativeInput)
    install_include_for_target(NativeInput Babylon)
endif()

if(TARGET NativeOptimizations)
    install_targets(NativeOptimizations)
    install_include_for_target(NativeOptimizations Babylon)
endif()

if(TARGET NativeTracing)
    install_targets(NativeTracing)
    install_include_for_target(NativeTracing)
endif()

if(TARGET NativeXr)
    install_targets(NativeXr)
    install_include_for_target(NativeXr)
endif()

# ----------------
# Polyfills
# ----------------

if(TARGET Canvas)
    install_targets(Canvas)
    install_include_for_target(Canvas)
endif()

if(TARGET Console)
    install_targets(Console)
    install_include_for_target(Console)
endif()

if(TARGET Window)
    install_targets(Window)
    install_include_for_target(Window)
endif()

if(TARGET XMLHttpRequest)
    install_targets(XMLHttpRequest)
    install_include_for_target(XMLHttpRequest)
endif()
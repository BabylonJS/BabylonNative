include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "..." FORCE)
endif()

function(install_targets)
    install(TARGETS ${ARGN})

    # Install the pdb files if they exist
    foreach(target IN LISTS ARGN)
        get_target_property(target_type ${target} TYPE)
        if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
            install(FILES "$<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_PREFIX:${target}>$<TARGET_FILE_BASE_NAME:${target}>.pdb" DESTINATION lib OPTIONAL)
        endif()
    endforeach()
endfunction()

function(install_include_for_targets)
    foreach(target IN LISTS ARGN)
        get_target_property(target_interface_include_directories ${target} INTERFACE_INCLUDE_DIRECTORIES)
        foreach(include_directory IN LISTS target_interface_include_directories)
            # Trailing slash is required for proper behavior
            # See https://cmake.org/cmake/help/latest/command/install.html#directory
            install(DIRECTORY "${include_directory}/" TYPE INCLUDE)
        endforeach()
    endforeach()
endfunction()

# ----------------
# Dependencies
# ----------------

## arcana.cpp
install_targets(arcana)

## bgfx
install_targets(bimg_encode bimg_decode bgfx bimg bx)

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

if(TARGET xr)
    install_targets(xr)
endif()

## UrlLib
install_targets(UrlLib)

## Fonudation
install_targets(Foundation)

# ----------------
# Core
# ----------------
install_include_for_targets(Foundation)

install_targets(JsRuntime)
install_include_for_targets(JsRuntime)

 # Note libs are in the `Graphics` target but includes are in `GraphicsDevice` target
install_targets(Graphics)
install_include_for_targets(GraphicsDevice)

if(TARGET AppRuntime)
    install_targets(AppRuntime)
    install_include_for_targets(AppRuntime)
endif()

if(TARGET Scheduling)
    install_targets(Scheduling)
    install_include_for_targets(Scheduling)
endif()

if(TARGET ScriptLoader)
    install_targets(ScriptLoader)
    install_include_for_targets(ScriptLoader)
endif()

install_targets(napi)
install_include_for_targets(napi)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    install_targets(llhttp_static v8inspector)
endif()

# Manually install the JSI headers
if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(V8JSI_VERSION "0.64.33")
    set(NUGET_PATH "${CMAKE_BINARY_DIR}/NuGet")
    if (WINDOWS_STORE)
        set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.UWP.${V8JSI_VERSION}")
    else()
        set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
    endif()
    install(DIRECTORY ${V8JSI_PACKAGE_PATH}/build/native/jsi/jsi TYPE INCLUDE)
endif()

install_targets(napi-extensions)
install_include_for_targets(napi-extensions)

# ----------------
# Plugins
# ----------------
if(TARGET ExternalTexture)
    install_targets(ExternalTexture)
    install_include_for_targets(ExternalTexture)
endif()

if(TARGET NativeCamera)
    install_targets(NativeCamera)
    install_include_for_targets(NativeCamera)
endif()

if(TARGET NativeCapture)
    install_targets(NativeCapture)
    install_include_for_targets(NativeCapture)
endif()

if(TARGET NativeEngine)
    install_targets(NativeEngine)
    install_include_for_targets(NativeEngine)
endif()

if(TARGET NativeInput)
    install_targets(NativeInput)
    install_include_for_targets(NativeInput)
endif()

if(TARGET NativeOptimizations)
    install_targets(NativeOptimizations)
    install_include_for_targets(NativeOptimizations)
endif()

if(TARGET NativeTracing)
    install_targets(NativeTracing)
    install_include_for_targets(NativeTracing)
endif()

if(TARGET NativeXr)
    install_targets(NativeXr)
    install_include_for_targets(NativeXr)
endif()

# ----------------
# Polyfills
# ----------------

if(TARGET Canvas)
    install_targets(Canvas)
    install_include_for_targets(Canvas)
endif()

if(TARGET Console)
    install_targets(Console)
    install_include_for_targets(Console)
endif()

if(TARGET Window)
    install_targets(Window)
    install_include_for_targets(Window)
endif()

if(TARGET XMLHttpRequest)
    install_targets(XMLHttpRequest)
    install_include_for_targets(XMLHttpRequest)
endif()

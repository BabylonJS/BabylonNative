include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "..." FORCE)
endif()

function(install_targets)
    foreach(target IN LISTS ARGN)
        get_target_property(target_type ${target} TYPE)
        if(NOT target_type STREQUAL "INTERFACE_LIBRARY")
            # Install the library file
            install(FILES "$<TARGET_FILE:${target}>" DESTINATION lib)

            # Install the pdb file if it exists
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
install_targets(bimg_encode bimg_decode bgfx bimg bx minz)

## glslang
install_targets(GenericCodeGen glslang glslang-default-resource-limits MachineIndependent OGLCompiler OSDependent SPIRV)

## libwebp
if(TARGET webp)
    install_targets(webp)
endif()

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

# ----------------
# Core
# ----------------

install_targets(Foundation)
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

if(TARGET NativeEncoding)
    install_targets(NativeEncoding)
    install_include_for_targets(NativeEncoding)
endif()

if(TARGET ShaderCache)
    install_targets(ShaderCache)
    install_include_for_targets(ShaderCache)
endif()

if(TARGET ShaderCompiler)
    install_targets(ShaderCompiler)
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

if(TARGET Blob)
    install_targets(Blob)
    install_include_for_targets(Blob)
endif()

if(TARGET URL)
    install_targets(URL)
    install_include_for_targets(URL)
endif()

if(TARGET AbortController)
    install_targets(AbortController)
    install_include_for_targets(AbortController)
endif()

if(TARGET WebSocket)
    install_targets(WebSocket)
    install_include_for_targets(WebSocket)
endif()

if(TARGET Performance)
    install_targets(Performance)
    install_include_for_targets(Performance)
endif()

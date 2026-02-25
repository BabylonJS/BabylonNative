include(GNUInstallDirs)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "..." FORCE)
endif()

function(install_lib)
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

function(install_bin)
    foreach(target IN LISTS ARGN)
        install(PROGRAMS "$<TARGET_FILE:${target}>" DESTINATION bin)
        install(FILES "$<TARGET_FILE_DIR:${target}>/$<TARGET_FILE_PREFIX:${target}>$<TARGET_FILE_BASE_NAME:${target}>.pdb" DESTINATION bin OPTIONAL)
    endforeach()
endfunction()

function(install_include)
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
install_lib(arcana)

## bgfx
install_lib(bimg_encode bimg_decode bgfx bimg bx minz)

## glslang
install_lib(GenericCodeGen glslang glslang-default-resource-limits MachineIndependent OGLCompiler OSDependent SPIRV)

## libwebp
if(TARGET webp)
    install_lib(webp)
endif()

## metal-cpp
if(APPLE)
    install_lib(metal-cpp)
    install_include(metal-cpp)
endif()

## SPIRV-Cross
install_lib(spirv-cross-core)
if(TARGET spirv-cross-glsl)
    install_lib(spirv-cross-glsl)
endif()
if(TARGET spirv-cross-msl)
    install_lib(spirv-cross-msl)
endif()
if(TARGET spirv-cross-hlsl)
    install_lib(spirv-cross-hlsl)
endif()

## UrlLib
install_lib(UrlLib)

## XR
if(TARGET openxr_loader)
    install_lib(openxr_loader)
endif()

if(TARGET xr)
    install_lib(xr)
endif()

# ----------------
# Core
# ----------------

if(TARGET AppRuntime)
    install_lib(AppRuntime)
    install_include(AppRuntime)
endif()

install_lib(Foundation)
install_include(Foundation)

# Note libs are in the `Graphics` target but includes are in `GraphicsDevice` target
install_lib(Graphics)
install_include(GraphicsDevice)

install_lib(JsRuntime)
install_include(JsRuntime)

install_lib(napi)
install_include(napi)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    install_lib(llhttp_static v8inspector)
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

install_lib(napi-extensions)
install_include(napi-extensions)

if(TARGET Scheduling)
    install_lib(Scheduling)
    install_include(Scheduling)
endif()

if(TARGET ScriptLoader)
    install_lib(ScriptLoader)
    install_include(ScriptLoader)
endif()

# ----------------
# Plugins
# ----------------

if(TARGET ExternalTexture)
    install_lib(ExternalTexture)
    install_include(ExternalTexture)
endif()

if(TARGET NativeCamera)
    install_lib(NativeCamera)
    install_include(NativeCamera)
endif()

if(TARGET NativeCapture)
    install_lib(NativeCapture)
    install_include(NativeCapture)
endif()

if(TARGET NativeEncoding)
    install_lib(NativeEncoding)
    install_include(NativeEncoding)
endif()

if(TARGET NativeEngine)
    install_lib(NativeEngine)
    install_include(NativeEngine)
endif()

if(TARGET NativeInput)
    install_lib(NativeInput)
    install_include(NativeInput)
endif()

if(TARGET NativeOptimizations)
    install_lib(NativeOptimizations)
    install_include(NativeOptimizations)
endif()

if(TARGET NativeTracing)
    install_lib(NativeTracing)
    install_include(NativeTracing)
endif()

if(TARGET NativeXr)
    install_lib(NativeXr)
    install_include(NativeXr)
endif()

if(TARGET ShaderCache)
    install_lib(ShaderCache)
    install_include(ShaderCache)
endif()

if(TARGET ShaderCompiler)
    install_lib(ShaderCompiler)
endif()

if(TARGET ShaderTool)
    install_bin(ShaderTool)
endif()

# ----------------
# Polyfills
# ----------------

if(TARGET AbortController)
    install_lib(AbortController)
    install_include(AbortController)
endif()

if(TARGET Blob)
    install_lib(Blob)
    install_include(Blob)
endif()

if(TARGET Canvas)
    install_lib(Canvas)
    install_include(Canvas)
endif()

if(TARGET Console)
    install_lib(Console)
    install_include(Console)
endif()

if(TARGET TextDecoder)
    install_lib(TextDecoder)
    install_include(TextDecoder)
endif()

if(TARGET Performance)
    install_lib(Performance)
    install_include(Performance)
endif()

if(TARGET URL)
    install_lib(URL)
    install_include(URL)
endif()

if(TARGET WebSocket)
    install_lib(WebSocket)
    install_include(WebSocket)
endif()

if(TARGET Window)
    install_lib(Window)
    install_include(Window)
endif()

if(TARGET XMLHttpRequest)
    install_lib(XMLHttpRequest)
    install_include(XMLHttpRequest)
endif()

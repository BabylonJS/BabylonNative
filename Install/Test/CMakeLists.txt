# cmake 3.18+ to have the ARCHIVE_EXTRACT sub-command for files
cmake_minimum_required(VERSION 3.18)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(TestInstall)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# __cplusplus value is not in sync with language version used. MVSC needs this flag to update it accordingly
# https://gitlab.kitware.com/cmake/cmake/-/issues/18837
if (MSVC)
    add_compile_options(/Zc:__cplusplus)
endif()

function(check_files_exist FILE_LIST)
    foreach(FILE ${${FILE_LIST}})
        if(NOT EXISTS "${INSTALL_LIBS_DIR}/${FILE}.lib")
            message(FATAL_ERROR "File '${FILE}' does not exist.")
        else()
            message(STATUS "File '${FILE}' exists.")
        endif()
    endforeach()
endfunction()

# executable stub
add_executable(TestInstall TestInstall.cpp)

# for napi
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

set(INSTALL_LIBS_DIR "${BINARY_DIR}/install/lib")
target_include_directories(TestInstall PRIVATE "${BINARY_DIR}/install/include")
target_link_directories(TestInstall PRIVATE ${INSTALL_LIBS_DIR})

# Nuget/JSI
# TODO: more it to generic cmake
set(NUGET_PATH "${BINARY_DIR}/NuGet")
set(V8JSI_VERSION "0.64.33")
set(CPU_ARCH "x64")
set(PLATFORM_FOLDER "win32")
set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
set(V8JSI_LIB_PATH_RELEASE "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Release/${CPU_ARCH}/")
target_link_directories(TestInstall PRIVATE "${V8JSI_LIB_PATH_RELEASE}")

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    set(EXTRA_LIBS "v8jsi.dll.lib")
endif()

set(STD_LIBS AppRuntime
    arcana
    astc-encoder
    bgfx
    bimg
    bx
    Canvas
    ChromeDevTools
    Console
    edtaa3
    etc1
    etc2
    ExternalTexture
    GenericCodeGen
    glslang
    Graphics
    iqa
    JsRuntime
    MachineIndependent
    napi
    NativeCamera
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
#    NativeXr
    nvtt
    OGLCompiler
    openxr_loader
    OSDependent
    pvrtc
    ScriptLoader
    spirv-cross-core
    spirv-cross-glsl
    spirv-cross-hlsl
    SPIRV
    squish
    tinyexr
    UrlLib
    Window
    XMLHttpRequest
    )

check_files_exist(STD_LIBS)

target_link_libraries(TestInstall PRIVATE 
    ${STD_LIBS}
    ${EXTRA_LIBS}
    chakrart
    d3d11
    d3d12
    d3dcompiler
    Pathcch
    )
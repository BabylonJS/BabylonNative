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

add_executable(TestInstall TestInstall.cpp)

# for napi
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(TestInstall PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

target_include_directories(TestInstall PRIVATE "${INSTALL_DIR}/include")
target_link_directories(TestInstall PRIVATE "${INSTALL_DIR}/lib")

target_link_libraries(TestInstall PRIVATE 
    AppRuntime
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
    chakrart
    d3d11
    d3d12
    d3dcompiler
    Pathcch
    )
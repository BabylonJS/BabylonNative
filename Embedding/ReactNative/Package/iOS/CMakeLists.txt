cmake_minimum_required(VERSION 3.13.2)
project(ReactNativeBabylon)
include(${CMAKE_CURRENT_SOURCE_DIR}/../../Apps/Playground/node_modules/@babylonjs/react-native/ios/CMakeLists.txt)

set(PACKAGED_LIBS
    astc-codec
    astc
    bgfx
    bimg
    bx
    GenericCodeGen
    glslang
    JsRuntime
    OGLCompiler
    OSDependent
    MachineIndependent
    napi
    NativeEngine
    NativeInput
    NativeWindow
    NativeXr
    SPIRV
    spirv-cross-core
    spirv-cross-glsl
    spirv-cross-hlsl
    spirv-cross-msl
    Window
    xr
)

foreach(packaged_lib ${PACKAGED_LIBS})
    add_custom_command(TARGET BabylonNative POST_BUILD
        COMMAND cp $<TARGET_FILE:${packaged_lib}> $<TARGET_FILE_DIR:BabylonNative>
        DEPENDS ${packaged_lib}
    )
endforeach()
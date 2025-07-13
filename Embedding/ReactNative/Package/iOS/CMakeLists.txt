cmake_minimum_required(VERSION 3.13.2)

include(FetchContent)

# Disable the WebP plugin option
set(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP OFF CACHE BOOL "Disable Babylon Native WebP Support")

FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.4.1)

FetchContent_MakeAvailable(ios-cmake)
set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
set(PLATFORM "OS64COMBINED" CACHE STRING "")
set(DEPLOYMENT_TARGET "12" CACHE STRING "")
set(ENABLE_ARC OFF CACHE STRING "Enables or disables ARC support.")
set(ENABLE_PCH OFF CACHE STRING "Enables or disables precompiled headers.")

project(ReactNativeBabylon)

# Clang will by default emit objc_msgSend stubs in Xcode 14, which ld from earlier Xcodes doesn't understand.
# We disable this by passing -fno-objc-msgsend-selector-stubs to clang.
# https://stackoverflow.com/questions/73983452/xcode-14-xcode-13-incompatibility
#add_compile_options("-fno-objc-msgsend-selector-stubs")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../Apps/${BUILD_RNAPP_DIR}/node_modules/@babylonjs/react-native-iosandroid/ios/CMakeLists.txt)

set_property(TARGET BabylonNative PROPERTY XCODE_GENERATE_SCHEME YES)

set(PACKAGED_LIBS
    bimg_encode
    bimg_decode
    bgfx
    bimg
    bx
    Canvas
    GenericCodeGen
    glslang
    glslang-default-resource-limits
    Graphics
    JsRuntime
    OGLCompiler
    OSDependent
    MachineIndependent
    napi
    NativeCamera
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    NativeXr
    SPIRV
    spirv-cross-core
    spirv-cross-msl
    UrlLib
    Window
    XMLHttpRequest
    Scheduling
    xr
)

foreach(packaged_lib ${PACKAGED_LIBS})
    add_custom_command(TARGET BabylonNative POST_BUILD
        COMMAND cp $<TARGET_FILE:${packaged_lib}> $<TARGET_FILE_DIR:BabylonNative>
        DEPENDS ${packaged_lib}
    )
endforeach()
cmake_minimum_required(VERSION 3.13.2)
project(ReactNativeBabylon)

set (CMAKE_VERBOSE_MAKEFILE ON)
set (CMAKE_CXX_STANDARD 20)


set (PACKAGE_NAME "ReactNativeBabylon")

# Clear some variables
unset(LIBRN_DIR CACHE)
unset(libfbjni_link_DIRS CACHE)
unset(libfbjni_include_DIRS CACHE)

set(build_DIR ${CMAKE_SOURCE_DIR}/build)
set(input_SRC src/main/cpp/BabylonNativeInterop.cpp)
file(GLOB libfbjni_link_DIRS "${build_DIR}/fbjni*.aar/jni/${ANDROID_ABI}")
file(GLOB libfbjni_include_DIRS "${build_DIR}/fbjni-*-headers.jar/")

message("-- ABI     : " ${ANDROID_ABI})
message("-- BUILD   : " ${build_DIR})
message("-- LIBRN   : " ${LIBRN_DIR})

link_directories(../libs/android/${ANDROID_ABI}/)

# Configure Babylon Native to use JSI
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "The JavaScript engine to power N-API")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/src/")

if(${REACT_NATIVE_VERSION} LESS 66)
    file(
            TO_CMAKE_PATH
            "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi/jsi/jsi.cpp"
            INCLUDE_JSI_CPP
    )
endif()

find_library(
        LOG_LIB
        log
)
message("-- LOG     : " ${LOG_LIB})

if(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # We need to find packages since from RN 0.71 binaries are prebuilt
    find_package(fbjni REQUIRED CONFIG)
    find_package(ReactAndroid REQUIRED CONFIG)
endif()

unset(JSI_LIB CACHE)
if(${REACT_NATIVE_VERSION} LESS 66)
    # JSI lib didn't exist on RN 0.65 and before. Simply omit it.
    set (JSI_LIB "")
elseif(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # RN 0.71 distributes prebuilt binaries.
    set (JSI_LIB ReactAndroid::jsi)
else()
    # RN 0.66 distributes libjsi.so, can be used instead of compiling jsi.cpp manually.
    find_library(
        JSI_LIB
        jsi
        PATHS ${LIBRN_DIR}
        NO_CMAKE_FIND_ROOT_PATH
    )
endif()
message("-- JSI     : " ${JSI_LIB})

add_library(jsi ALIAS ReactAndroid::jsi)

set(BABYLON_NATIVE_BUILD_APPS OFF CACHE BOOL "")
set(BABYLON_REACT_NATIVE_SHARED_DIR "${CMAKE_CURRENT_LIST_DIR}/../shared")
set(BABYLON_NATIVE_PLUGIN_NATIVEENGINE_WEBP OFF CACHE BOOL "")
add_subdirectory(${BABYLON_REACT_NATIVE_SHARED_DIR} ${CMAKE_CURRENT_BINARY_DIR}/shared)

if (TARGET NativeXr)
    disable_warnings(NativeXr)
endif()
if (TARGET NativeCamera)
    disable_warnings(NativeCamera)
endif()
disable_warnings(XMLHttpRequest)
disable_warnings(Graphics)
disable_warnings(NativeEngine)
disable_warnings(NativeInput)
disable_warnings(NativeCapture)
disable_warnings(NativeOptimizations) # deprecated stl
disable_warnings(NativeTracing) # deprecated stl
disable_warnings(JsRuntime) # deprecated stl
disable_warnings(Scheduling) # deprecated stl
disable_warnings(Canvas)
disable_warnings(Window)

file(GLOB BABYLON_SHARED "${CMAKE_CURRENT_SOURCE_DIR}/../shared/*")

unset(REACT_LIB CACHE)
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 76)
    # RN 0.76 packs react_nativemodule_core into ReactAndroid::reactnative
    set (REACT_LIB ReactAndroid::reactnative)
elseif(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # RN 0.71 distributes prebuilt binaries.
    set (REACT_LIB ReactAndroid::react_nativemodule_core)
    else()
    find_library(
            REACT_LIB
            react_nativemodule_core
            PATHS ${LIBRN_DIR}
            NO_CMAKE_FIND_ROOT_PATH
    )
endif()
message("-- REACT   : " ${REACT_LIB})

unset(FBJNI_LIBRARY CACHE)
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # RN 0.71 distributes prebuilt binaries.
    set (FBJNI_LIBRARY fbjni::fbjni)
else()
    find_library(
            FBJNI_LIBRARY
            fbjni
            PATHS ${libfbjni_link_DIRS}
            NO_CMAKE_FIND_ROOT_PATH
    )
endif()
message("-- FBJNI   : " ${FBJNI_LIBRARY})

unset(REACTNATIVEJNI_LIB CACHE)
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 76)
    # RN 0.76 doesn't have reactnativejni
    # DO NOTHING, we'll not link these libraries
elseif(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # RN 0.71 distributes prebuilt binaries.
    set (REACTNATIVEJNI_LIB "ReactAndroid::reactnativejni")
else()
    find_library(
            REACTNATIVEJNI_LIB
            reactnativejni
            PATHS ${LIBRN_DIR}
            NO_CMAKE_FIND_ROOT_PATH
    )
endif()
message("-- REACTNATIVEJNI   : " ${REACTNATIVEJNI_LIB})

unset(RUNTIMEEXECUTOR_LIB CACHE)
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 76)
    # RN 0.76 doesn't have runtimeexecutor
    # DO NOTHING, we'll not link these libraries
elseif(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # RN 0.71 distributes prebuilt binaries.
    set (RUNTIMEEXECUTOR_LIB "ReactAndroid::runtimeexecutor")
else()
    find_library(
            RUNTIMEEXECUTOR_LIB
            runtimeexecutor
            PATHS ${LIBRN_DIR}
            NO_CMAKE_FIND_ROOT_PATH
    )
endif()
message("-- RUNTIMEEXECUTOR   : " ${RUNTIMEEXECUTOR_LIB})

unset(TURBOMODULES_LIB CACHE)
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 76)
    # RN 0.76 doesn't have turbomodulejsijni
    # DO NOTHING, we'll not link these libraries
elseif(${REACT_NATIVE_VERSION} GREATER_EQUAL 71)
    # RN 0.71 distributes prebuilt binaries.
    set (TURBOMODULES_LIB "ReactAndroid::turbomodulejsijni")
else()
    find_library(
            TURBOMODULES_LIB
            turbomodulejsijni
            PATHS ${LIBRN_DIR}
            NO_CMAKE_FIND_ROOT_PATH
    )
endif()
message("-- TURBO   : " ${TURBOMODULES_LIB})

add_definitions(-DREACT_NATIVE_VERSION=${REACT_NATIVE_VERSION})

# Link
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 76)
    set(COMMON_LIBS
        ${LOG_LIB}
        ${REACT_LIB}
        ${FBJNI_LIBRARY}
        ${JSI_LIB}
        -ljnigraphics
        -lGLESv2
        -lEGL
        -landroid
    )
else()
    set(COMMON_LIBS
        ${LOG_LIB}
        ${FBJNI_LIBRARY}
        ${REACT_LIB}
        ${JSI_LIB}
        ${REACTNATIVEJNI_LIB}
        ${RUNTIMEEXECUTOR_LIB}
        ${TURBOMODULES_LIB}
        -ljnigraphics
        -lGLESv2
        -lEGL
        -landroid
    )
endif()


add_library(
        ${PACKAGE_NAME}
        SHARED
        "${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/BabylonNativeInterop.cpp"
        ${BABYLON_SHARED}
)
disable_warnings(${PACKAGE_NAME}) # deprecated stl

# Set RN_SERIALIZABLE_STATE and C++ flags for React Native 0.81+
if(${REACT_NATIVE_VERSION} GREATER_EQUAL 80)
    include("${REACT_NATIVE_DIR}/ReactCommon/cmake-utils/react-native-flags.cmake")
    target_compile_reactnative_options(${PACKAGE_NAME} PRIVATE)
else()
  string(APPEND CMAKE_CXX_FLAGS
         "  -fexceptions -frtti -std=c++${CMAKE_CXX_STANDARD} -Wall -Werror -Wunused-function -Wunused-private-field -Woverloaded-virtual -Wreorder-ctor -Wdelete-non-abstract-non-virtual-dtor -Wmismatched-tags -Wunused-variable -Wpessimizing-move -Wswitch -Wdeprecated-declarations -Werror=deprecated-this-capture")
endif()

target_include_directories(
        ${PACKAGE_NAME}
        PRIVATE

        "${NODE_MODULES_DIR}/react-native/ReactCommon/callinvoker"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/jsi"
        "${NODE_MODULES_DIR}/react-native/ReactCommon"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/react/nativemodule/core"
        "${NODE_MODULES_DIR}/react-native/ReactCommon/runtimeexecutor"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/jni"
        "${NODE_MODULES_DIR}/react-native/ReactAndroid/src/main/java/com/facebook/react/turbomodule/core/jni"

        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../shared"
        PRIVATE $<TARGET_PROPERTY:Graphics,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeEngine,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeInput,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:Window,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:XMLHttpRequest,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeCamera,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeXr,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeCapture,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeOptimizations,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:NativeTracing,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:Canvas,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:ShaderCache,INCLUDE_DIRECTORIES>
        PRIVATE $<TARGET_PROPERTY:ShaderCompiler,INCLUDE_DIRECTORIES>

        ${libfbjni_include_DIRS}
)

target_link_libraries(${PACKAGE_NAME}
    ${COMMON_LIBS}
    AndroidExtensions
    Graphics
    NativeEngine
    Canvas
    XMLHttpRequest
    NativeInput
    NativeCapture
    NativeOptimizations
    NativeTracing
    Window
    NativeXr
    ShaderCache
    ShaderCompiler
    NativeCamera
    -lGLESv3
    -lEGL
)

# Enable Android 16kb native library alignment
if(CMAKE_ANDROID_NDK_VERSION VERSION_LESS "27")
    target_link_options(${PACKAGE_NAME} PRIVATE "-Wl,-z,max-page-size=16384")
endif()

target_compile_definitions(${PACKAGE_NAME} PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEXR=${BABYLON_NATIVE_PLUGIN_NATIVEXR})
target_compile_definitions(${PACKAGE_NAME} PRIVATE BABYLON_NATIVE_PLUGIN_NATIVECAMERA=${BABYLON_NATIVE_PLUGIN_NATIVECAMERA})

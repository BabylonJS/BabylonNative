cmake_minimum_required(VERSION 3.15.3)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

project(ReactNativeBabylon)

set(CMAKE_CXX_STANDARD 17)

# BabylonNative CMake entry
set(BABYLON_NATIVE_BUILD_APPS OFF CACHE BOOL "")
set(BABYLON_NATIVE_USE_SWAPCHAINPANEL ON CACHE BOOL "")

# Force all cmake-built targets to use the same C++/WinRT headers as the vcxproj.
# The vcxproj uses the CppWinRT NuGet package version pinned by react-native-windows
# (CppWinRTVersion, typically 2.0.230706.1).
# (CppWinRTVersion, typically 2.0.230706.1). Without this, cmake falls back to the
# Windows SDK CppWinRT which may be a newer version, causing a #pragma detect_mismatch
# link error ("C++/WinRT version" mismatch at link time).
# CPPWINRT_INCLUDE_PATH is passed by the BabylonNativeBuild MSBuild target in the vcxproj:
#   -DCPPWINRT_INCLUDE_PATH="$(NuGetPackageRoot)microsoft.windows.cppwinrt\$(CppWinRTVersion)\build\native\include"
if(DEFINED CPPWINRT_INCLUDE_PATH AND EXISTS "${CPPWINRT_INCLUDE_PATH}")
    message("-- CppWinRT: Forcing include path: ${CPPWINRT_INCLUDE_PATH}")
    include_directories(BEFORE "${CPPWINRT_INCLUDE_PATH}")
else()
    message(WARNING "CppWinRT: CPPWINRT_INCLUDE_PATH not set or not found ('${CPPWINRT_INCLUDE_PATH}'). "
        "cmake-built targets may use a different CppWinRT version than the vcxproj, "
        "causing a C++/WinRT version detect_mismatch link error.")
endif()

# Configure Babylon Native to use JSI
# Note: We should avoid installing node_modules in the Module\@babylonjs\react-native folder.
# Installing react-native dependencies for both the Playground app and the @babylonjs/react-native package will generate a bad bundle/runtime errors.
set(NAPI_JAVASCRIPT_ENGINE "JSI" CACHE STRING "")

set(PLAYGROUND_DIR "${CMAKE_CURRENT_LIST_DIR}/../../..")

if(EXISTS "${PLAYGROUND_DIR}/react-native/package.json")
    get_filename_component(REACTNATIVE_DIR_CMAKE "${PLAYGROUND_DIR}/react-native" ABSOLUTE)
else()
    message("Playground directory ${PLAYGROUND_DIR}")
    message(FATAL_ERROR "No Playground available")
endif()

add_subdirectory("${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi/jsi" ${CMAKE_CURRENT_BINARY_DIR}/jsi)
target_include_directories(jsi INTERFACE ${REACTNATIVE_DIR_CMAKE}/ReactCommon/jsi)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/../shared ${CMAKE_CURRENT_BINARY_DIR}/shared)

# Disable Unity build for UrlLib because of conflict in header between windows.h and winrt
set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)

add_library(BabylonNative ${SHARED_SOURCES})

target_compile_definitions(BabylonNative PRIVATE BABYLON_NATIVE_PLUGIN_NATIVEXR=${BABYLON_NATIVE_PLUGIN_NATIVEXR})
target_compile_definitions(BabylonNative PRIVATE BABYLON_NATIVE_PLUGIN_NATIVECAMERA=${BABYLON_NATIVE_PLUGIN_NATIVECAMERA})
target_include_directories(BabylonNative PRIVATE ${SHARED_INCLUDES})

# because of warning C5030: attribute 'msvc::intrinsic' is not recognized
# solution is to update VS version but this might bring more issues
disable_warnings(Graphics)
disable_warnings(NativeEngine)
disable_warnings(NativeCamera)
disable_warnings(NativeInput)
disable_warnings(NativeCapture)
disable_warnings(Canvas)
disable_warnings(Window)
disable_warnings(ExternalTexture)

target_link_libraries(BabylonNative
    arcana
    GraphicsDevice
    jsi
    JsRuntime
    NativeCapture
    NativeEngine
    NativeInput
    NativeOptimizations
    NativeTracing
    Window
    Scheduling
    XMLHttpRequest
    ShaderCache
    Canvas
    ${ADDITIONAL_LIBRARIES})


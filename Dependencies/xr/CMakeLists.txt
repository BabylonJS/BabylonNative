cmake_minimum_required(VERSION 3.12)

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.4.1)
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

if(IOS)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "13" CACHE STRING "")
endif()

project(xr)

set(DYNAMIC_LOADER OFF CACHE BOOL "Build the loader as a .dll library")

set(SOURCES "Include/XR.h"
            "Source/Shared/XRHelpers.h")

if (ANDROID)
    set(SOURCES ${SOURCES}
        "Source/ARCore/Include/IXrContextARCore.h"
        "Source/ARCore/XR.cpp")
elseif(IOS OR BABYLON_NATIVE_BUILD_SOURCETREE)
    set(SOURCES ${SOURCES}
        "Source/ARKit/Include/IXrContextARKit.h"
        "Source/ARKit/XR.mm")
endif()

add_library(xr ${SOURCES})
add_library(xrInternal INTERFACE)

target_compile_definitions(xr PRIVATE _CRT_SECURE_NO_WARNINGS)

if (APPLE)
    set_target_properties(xr PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    )
    set_property(TARGET xr PROPERTY UNITY_BUILD false)
endif()

if (ANDROID)
    add_library(arcore SHARED IMPORTED)
    set_target_properties(arcore PROPERTIES IMPORTED_LOCATION
                          ${ARCORE_LIBPATH}/${ANDROID_ABI}/libarcore_sdk_c.so
                          INTERFACE_INCLUDE_DIRECTORIES ${arcore-android-sdk_SOURCE_DIR}/libraries/include)

    add_library(glm INTERFACE)
    set_target_properties(glm PROPERTIES INTERFACE_INCLUDE_DIRECTORIES ${arcore-android-sdk_SOURCE_DIR}/libraries/glm)

    target_link_libraries(xr PRIVATE glm arcore AndroidExtensions)
elseif (IOS)    
    target_link_libraries(xr PRIVATE "-framework ARKit")
endif()

target_link_libraries(xr
    PUBLIC arcana)

target_include_directories(xr PUBLIC "Include")
target_include_directories(xr PRIVATE "Source/Shared")

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

target_include_directories(xrInternal INTERFACE "Source")
target_link_libraries(xrInternal
    INTERFACE xr)

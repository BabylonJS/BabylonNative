cmake_minimum_required(VERSION 3.18)

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG        f2757396e80bc4169f2ddb938ce25367a98ffdd0)
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/bghgary/AndroidExtensions.git
    GIT_TAG        7a20de1f3f39e24f59a29b1f4e942668e50ad0e6)
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

project(UrlLib)

FetchContent_MakeAvailable_With_Message(arcana.cpp)

if(ANDROID)
    FetchContent_MakeAvailable_With_Message(AndroidExtensions)
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ANDROID)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Android.cpp"
        "Source/WebSocket_Android.cpp")
    set(ADDITIONAL_LIBRARIES
        PRIVATE android
        PRIVATE AndroidExtensions
        PRIVATE log)
elseif(APPLE)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Apple.mm"
        "Source/WebSocket_Apple.mm"
        "Source/WebSocket_Apple_ObjC.h"
        "Source/WebSocket_Apple_ObjC.m")
elseif(UNIX)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Unix.cpp"
        "Source/WebSocket_Unix.cpp")
    set(ADDITIONAL_LIBRARIES
        PRIVATE curl)
elseif(WIN32)
    set(ADDITIONAL_SOURCES
        "Source/UrlRequest_Windows_Shared.h"
        "Source/WebSocket_Windows.cpp")
    if(WINDOWS_STORE)
        set(ADDITIONAL_SOURCES ${ADDITIONAL_SOURCES}
            "Source/UrlRequest_UWP.cpp")
    else()
        set(ADDITIONAL_SOURCES ${ADDITIONAL_SOURCES}
            "Source/UrlRequest_Win32.cpp")
        set(ADDITIONAL_LIBRARIES
            PUBLIC onecoreuap.lib)
    endif()
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

set(SOURCES
    "Include/UrlLib/UrlLib.h"
    "Source/UrlRequest_Base.h"
    "Source/UrlRequest_Shared.h"
    "Source/WebSocket_Base.h"
    "Source/WebSocket_Shared.h"
    ${ADDITIONAL_SOURCES})

add_library(UrlLib ${SOURCES})

target_include_directories(UrlLib PUBLIC "Include")
target_include_directories(UrlLib PRIVATE "Source")

target_link_libraries(UrlLib
    PUBLIC arcana
    ${ADDITIONAL_LIBRARIES})

if(APPLE)
    set_target_properties(UrlLib PROPERTIES
        XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES)
    set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

cmake_minimum_required(VERSION 3.18)

include(FetchContent)
include("${CMAKE_CURRENT_LIST_DIR}/../CMakeExtensions/CMakeLists.txt")

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG        7c6be8aaa29effbc8ee1a2217388fb6e399150d2)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.4.1)
# --------------------------------------------------

if(IOS)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "13" CACHE STRING "")
endif()

project(UrlLib)

FetchContent_MakeAvailable_With_Message(arcana.cpp)

if(ANDROID)
    if(NOT TARGET AndroidExtensions)
        add_subdirectory("../AndroidExtensions" "${CMAKE_BINARY_DIR}/_deps/androidextensions-build" EXCLUDE_FROM_ALL)
    endif()
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 20)
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
    enable_objc_arc(UrlLib)
    set_property(TARGET UrlLib PROPERTY UNITY_BUILD false)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

# Standalone desktop builds enable tests by default; BabylonNative explicitly
# enables them for its desktop test configurations.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR AND NOT IOS AND NOT ANDROID)
    option(URLLIB_TESTS "Build the UrlLib unit tests." ON)
else()
    option(URLLIB_TESTS "Build the UrlLib unit tests." OFF)
endif()

if(URLLIB_TESTS)
    enable_testing()
    add_subdirectory(Tests)
endif()

cmake_minimum_required(VERSION 3.18)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UrlLib)

include(FetchContent)

if(NOT TARGET arcana)
    include(FetchContent)

    FetchContent_Declare(
        arcana
        GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
        GIT_TAG        10d167ffe0f86b2ddaccd93fd78ff2240cbb0c49)

    message(STATUS "Fetching arcana.cpp")
    FetchContent_MakeAvailable(arcana)
    message(STATUS "Fetching arcana.cpp - done")
endif()

if(ANDROID AND NOT TARGET AndroidExtensions)
    FetchContent_Declare(
        AndroidExtensions
        GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
        GIT_TAG        c7c8b44a9d021ee5e1159c4fec98d2ef0735401a)

    message(STATUS "Fetching AndroidExtensions")
    FetchContent_MakeAvailable(AndroidExtensions)
    message(STATUS "Fetching AndroidExtensions - done")
endif()

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
        "Source/UrlRequest_Windows.cpp"
        "Source/WebSocket_Windows.cpp")
    if(NOT WINDOWS_STORE)
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

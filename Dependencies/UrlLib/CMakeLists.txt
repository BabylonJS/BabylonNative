cmake_minimum_required(VERSION 3.18)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UrlLib)

include(FetchContent)

# Modules declaration
FetchContent_Declare(arcana
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG        f2757396e80bc4169f2ddb938ce25367a98ffdd0)
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
    GIT_TAG        1a47db416ec2aae3f51b28b94f73e8f54e412d0d)

if(NOT TARGET arcana)
    set(CONTENT_TO_FETCH ${CONTENT_TO_FETCH} arcana)
endif()

if(ANDROID AND NOT TARGET AndroidExtensions)
    set(CONTENT_TO_FETCH ${CONTENT_TO_FETCH} AndroidExtensions)
endif()

# Fetching content
message(STATUS "Fetching dependencies for ${PROJECT_NAME} (${CONTENT_TO_FETCH})")
FetchContent_MakeAvailable(${CONTENT_TO_FETCH})
message(STATUS "Fetching dependencies for ${PROJECT_NAME} - done")

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

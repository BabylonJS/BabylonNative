cmake_minimum_required(VERSION 3.18)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(JsRuntimeHost)

# Options

## Core
option(JSRUNTIMEHOST_CORE_APPRUNTIME "Include Babylon Native Core AppRuntime" ON)
option(JSRUNTIMEHOST_CORE_SCRIPTLOADER "Include Babylon Native Core ScriptLoader" ON)

## Polyfills
option(JSRUNTIMEHOST_POLYFILL_CONSOLE "Include Babylon Native Polyfill Console." ON)
option(JSRUNTIMEHOST_POLYFILL_SCHEDULING "Include Babylon Native Polyfill Scheduling." ON)
option(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST "Include Babylon Native Polyfill XMLHttpRequest." ON)

## Tests
option(JSRUNTIMEHOST_TESTS "Include Babylon Native Tests." ON)

# Dependencies

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

    set_property(TARGET arcana PROPERTY FOLDER Dependencies)
endif()

if(NOT TARGET UrlLib AND JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    FetchContent_Declare(
        UrlLib
        GIT_REPOSITORY https://github.com/BabylonJS/UrlLib.git
        GIT_TAG        0007501304a710f14626f34a73c05bcb371de92f)

    message(STATUS "Fetching UrlLib")
    FetchContent_MakeAvailable(UrlLib)
    message(STATUS "Fetching UrlLib - done")

    set_property(TARGET UrlLib PROPERTY FOLDER Dependencies)
endif()

# Subdirectories

if(ANDROID)
    set(JSRUNTIMEHOST_PLATFORM "Android")
elseif(IOS)
    set(JSRUNTIMEHOST_PLATFORM "iOS")
elseif(APPLE)
    set(JSRUNTIMEHOST_PLATFORM "macOS")
elseif(WINDOWS_STORE)
    set(JSRUNTIMEHOST_PLATFORM "UWP")
elseif(WIN32)
    set(JSRUNTIMEHOST_PLATFORM "Win32")
elseif(UNIX)
    set(JSRUNTIMEHOST_PLATFORM "Unix")
else()
    message(FATAL_ERROR "Unrecognized platform: ${CMAKE_SYSTEM_NAME}")
endif()

if(APPLE)
    set(JSRUNTIMEHOST_PLATFORM_IMPL_EXT "mm")
else()
    set(JSRUNTIMEHOST_PLATFORM_IMPL_EXT "cpp")
endif()

add_subdirectory(Core)
add_subdirectory(Polyfills)

if(JSRUNTIMEHOST_TESTS)
    add_subdirectory(Tests)
endif()

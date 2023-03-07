cmake_minimum_required(VERSION 3.18)

include(FetchContent)

if(IOS)
    FetchContent_Declare(
        ios-cmake
        GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
        GIT_TAG        04d91f6675dabb3c97df346a32f6184b0a7ef845)

    message(STATUS "Fetching ios-cmake")
    FetchContent_MakeAvailable(ios-cmake)
    message(STATUS "Fetching ios-cmake - done")

    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "12" CACHE STRING "")
endif()

project(JsRuntimeHost)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options

## Core
option(JSRUNTIMEHOST_CORE_APPRUNTIME "Include Babylon Native Core AppRuntime" ON)
option(JSRUNTIMEHOST_CORE_SCRIPTLOADER "Include Babylon Native Core ScriptLoader" ON)

## Polyfills
option(JSRUNTIMEHOST_POLYFILL_CONSOLE "Include Babylon Native Polyfill Console." ON)
option(JSRUNTIMEHOST_POLYFILL_SCHEDULING "Include Babylon Native Polyfill Scheduling." ON)
option(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST "Include Babylon Native Polyfill XMLHttpRequest." ON)
option(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER "Include Babylon Native Polyfills AbortController and AbortSignal." ON)

## Tests
option(JSRUNTIMEHOST_TESTS "Include Babylon Native Tests." ON)

# Dependencies

if(NOT TARGET arcana)
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
        GIT_TAG        6413ba699107d8114ca19e7646638d3c628eb956)

    message(STATUS "Fetching UrlLib")
    FetchContent_MakeAvailable(UrlLib)
    message(STATUS "Fetching UrlLib - done")

    set_property(TARGET UrlLib PROPERTY FOLDER Dependencies)
endif()

# Common Variables

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(NPM_COMMAND "npm.cmd")
else()
    set(NPM_COMMAND "npm")
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

add_subdirectory(Core)
add_subdirectory(Polyfills)

if(JSRUNTIMEHOST_TESTS)
    add_subdirectory(Tests)
endif()

cmake_minimum_required(VERSION 3.18)

# Avoid warning about DOWNLOAD_EXTRACT_TIMESTAMP in CMake 3.24
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

if(IOS)
    FetchContent_Declare(ios-cmake
        GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
        GIT_TAG 04d91f6675dabb3c97df346a32f6184b0a7ef845)

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
option(JSRUNTIMEHOST_CORE_APPRUNTIME "Include JsRuntimeHost Core AppRuntime" ON)
option(JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR "Include the V8 inspector protocol required to debug JavaScript with a V8 debugger." ON)
option(JSRUNTIMEHOST_CORE_SCRIPTLOADER "Include JsRuntimeHost Core ScriptLoader" ON)

## Polyfills
option(JSRUNTIMEHOST_POLYFILL_CONSOLE "Include JsRuntimeHost Polyfill Console." ON)
option(JSRUNTIMEHOST_POLYFILL_SCHEDULING "Include JsRuntimeHost Polyfill Scheduling." ON)
option(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST "Include JsRuntimeHost Polyfill XMLHttpRequest." ON)
option(JSRUNTIMEHOST_POLYFILL_URL "Include JsRuntimeHost Polyfill URL and URLSearchParams." ON)
option(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER "Include JsRuntimeHost Polyfills AbortController and AbortSignal." ON)
option(JSRUNTIMEHOST_POLYFILL_WEBSOCKET "Include JsRuntimeHost Polyfill WebSocket." ON)

## Tests
option(JSRUNTIMEHOST_TESTS "Include JsRuntimeHost Tests." ON)

## Node-API
option(NAPI_BUILD_ABI "Build the ABI layer." ON)

# Dependencies

if(NOT TARGET arcana)
    FetchContent_Declare(arcana
        GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
        GIT_TAG 10d167ffe0f86b2ddaccd93fd78ff2240cbb0c49)

    message(STATUS "Fetching arcana.cpp")
    FetchContent_MakeAvailable(arcana)
    message(STATUS "Fetching arcana.cpp - done")

    set_property(TARGET arcana PROPERTY FOLDER Dependencies)
endif()

if(NOT TARGET UrlLib AND JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    FetchContent_Declare(
        UrlLib
        GIT_REPOSITORY https://github.com/BabylonJS/UrlLib.git
        GIT_TAG f4bae6043c690132a6b219ec80f68f0c252aaa7d)

    message(STATUS "Fetching UrlLib")
    FetchContent_MakeAvailable(UrlLib)
    message(STATUS "Fetching UrlLib - done")

    set_property(TARGET UrlLib PROPERTY FOLDER Dependencies)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    if(NOT TARGET asio)
        FetchContent_Declare(asio
            GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
            GIT_TAG f693a3eb7fe72a5f19b975289afc4f437d373d9c)

        message(STATUS "Fetching asio")
        FetchContent_MakeAvailable(asio)
        message(STATUS "Fetching asio - done")

        add_library(asio INTERFACE)
        target_include_directories(asio INTERFACE "${asio_SOURCE_DIR}/asio/include")

        set_property(TARGET asio PROPERTY FOLDER Dependencies)
    endif()

    if(NOT TARGET llhttp_static)
        FetchContent_Declare(llhttp
            URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz")

        message(STATUS "Fetching llhttp")
        set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
        set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
        FetchContent_MakeAvailable(llhttp)
        message(STATUS "Fetching llhttp - done")

        set_property(TARGET llhttp_static PROPERTY FOLDER Dependencies)
    endif()
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

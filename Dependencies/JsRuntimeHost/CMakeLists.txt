cmake_minimum_required(VERSION 3.21)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

include(FetchContent)

# --------------------------------------------------
# Declarations
# --------------------------------------------------
FetchContent_Declare(AndroidExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/AndroidExtensions.git
    GIT_TAG f7ed149b5360cc8a4908fece66607c5ce1e6095b
    EXCLUDE_FROM_ALL)
FetchContent_Declare(arcana.cpp
    GIT_REPOSITORY https://github.com/microsoft/arcana.cpp.git
    GIT_TAG c02527c32d51b6b3ffeda36f8cf140d2e1c60bb9
    EXCLUDE_FROM_ALL)
FetchContent_Declare(asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG f693a3eb7fe72a5f19b975289afc4f437d373d9c
    EXCLUDE_FROM_ALL)
FetchContent_Declare(CMakeExtensions
    GIT_REPOSITORY https://github.com/BabylonJS/CMakeExtensions.git
    GIT_TAG ea28b7689530bfdc4905806f27ecf7e8ed4b5419
    EXCLUDE_FROM_ALL)
FetchContent_Declare(googletest
    URL "https://github.com/google/googletest/archive/refs/tags/v1.17.0.tar.gz"
    EXCLUDE_FROM_ALL)
FetchContent_Declare(ios-cmake
    GIT_REPOSITORY https://github.com/leetal/ios-cmake.git
    GIT_TAG 4.4.1
    EXCLUDE_FROM_ALL)
FetchContent_Declare(llhttp
    URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v8.1.0.tar.gz"
    EXCLUDE_FROM_ALL)
FetchContent_Declare(UrlLib
    GIT_REPOSITORY https://github.com/BabylonJS/UrlLib.git
    GIT_TAG 1f9d2c05f792d994b09fdfc1129b25294efbe182
    EXCLUDE_FROM_ALL)
# --------------------------------------------------

FetchContent_MakeAvailable(CMakeExtensions)

if(IOS)
    FetchContent_MakeAvailable_With_Message(ios-cmake)
    set(CMAKE_TOOLCHAIN_FILE "${ios-cmake_SOURCE_DIR}/ios.toolchain.cmake" CACHE PATH "")
    set(PLATFORM "OS64COMBINED" CACHE STRING "")
    set(DEPLOYMENT_TARGET "12" CACHE STRING "")
    set(CMAKE_XCODE_GENERATE_SCHEME TRUE)
endif()

project(JsRuntimeHost)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# Options
# --------------------------------------------------

# General
option(JSRUNTIMEHOST_TESTS "Include JsRuntimeHost Tests." ${PROJECT_IS_TOP_LEVEL})
option(NAPI_BUILD_ABI "Build the ABI layer." ON)
option(BABYLON_DEBUG_TRACE "Debug Trace callback."  OFF)

# Core
option(JSRUNTIMEHOST_CORE_APPRUNTIME "Include JsRuntimeHost Core AppRuntime" ON)
option(JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR "Include the V8 inspector protocol required to debug JavaScript with a V8 debugger." ON)
option(JSRUNTIMEHOST_CORE_SCRIPTLOADER "Include JsRuntimeHost Core ScriptLoader" ON)

# Polyfills
option(JSRUNTIMEHOST_POLYFILL_CONSOLE "Include JsRuntimeHost Polyfill Console." ON)
option(JSRUNTIMEHOST_POLYFILL_SCHEDULING "Include JsRuntimeHost Polyfill Scheduling." ON)
option(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST "Include JsRuntimeHost Polyfill XMLHttpRequest." ON)
option(JSRUNTIMEHOST_POLYFILL_URL "Include JsRuntimeHost Polyfill URL and URLSearchParams." ON)
option(JSRUNTIMEHOST_POLYFILL_ABORT_CONTROLLER "Include JsRuntimeHost Polyfills AbortController and AbortSignal." ON)
option(JSRUNTIMEHOST_POLYFILL_WEBSOCKET "Include JsRuntimeHost Polyfill WebSocket." ON)
option(JSRUNTIMEHOST_POLYFILL_BLOB "Include JsRuntimeHost Polyfill Blob." ON)
option(JSRUNTIMEHOST_POLYFILL_PERFORMANCE "Include JsRuntimeHost Polyfill Performance." ON)

# Sanitizers
option(ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

if(ENABLE_SANITIZERS)
    set(ENABLE_RTTI ON CACHE BOOL "" FORCE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        set(SANITIZERS "address,undefined")
        # Check for Clang since vptr and fdsan are Clang-specific
        if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
            list(APPEND SANITIZERS "vptr")
            # FDSan only works on Android builds with Clang
            if (ANDROID)
                list(APPEND SANITIZERS "fdsan")
            endif()
        endif()

        string(JOIN "," SANITIZER_FLAGS ${SANITIZERS})

        add_compile_options(-fsanitize=${SANITIZER_FLAGS} -fno-omit-frame-pointer)
        add_link_options(-fsanitize=${SANITIZER_FLAGS})
    else()
        message(WARNING "Sanitizers not supported on this compiler.")
    endif()
endif()

# --------------------------------------------------

FetchContent_MakeAvailable_With_Message(arcana.cpp)
set_property(TARGET arcana PROPERTY FOLDER Dependencies)

if(JSRUNTIMEHOST_POLYFILL_XMLHTTPREQUEST)
    FetchContent_MakeAvailable_With_Message(UrlLib)
    set_property(TARGET UrlLib PROPERTY FOLDER Dependencies)
endif()

if(BABYLON_DEBUG_TRACE)
    add_definitions(-DBABYLON_DEBUG_TRACE)
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    FetchContent_MakeAvailable_With_Message(asio)
    add_library(asio INTERFACE)
    target_include_directories(asio INTERFACE "${asio_SOURCE_DIR}/asio/include")
    set_property(TARGET asio PROPERTY FOLDER Dependencies)

    set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
    set(BUILD_STATIC_LIBS ON CACHE INTERNAL "")
    FetchContent_MakeAvailable_With_Message(llhttp)
    set_property(TARGET llhttp_static PROPERTY FOLDER Dependencies)
endif()

if(JSRUNTIMEHOST_TESTS)
    if(WIN32)
        # For Windows: Prevent overriding the parent project's compiler/linker settings
        # Default build type for my test projects are /MDd (MultiThreaded DLL) but GTests default to /MTd (MultiThreaded)
        # see https://github.com/google/googletest/blob/main/googletest/README.md
        # "Enabling this option will make gtest link the runtimes dynamically too, and match the project in which it is included."
        set(gtest_force_shared_crt OFF CACHE BOOL "" FORCE)
    endif()

    FetchContent_MakeAvailable_With_Message(googletest)

    set_property(TARGET gmock PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gmock_main PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest PROPERTY FOLDER Dependencies/GoogleTest)
    set_property(TARGET gtest_main PROPERTY FOLDER Dependencies/GoogleTest)
endif()

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

if(JSRUNTIMEHOST_TESTS AND NOT WINDOWS_STORE)
    add_subdirectory(Tests)
endif()

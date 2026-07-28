cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UnitTestsJNI)

get_filename_component(UNIT_TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
get_filename_component(REPO_ROOT_DIR "${UNIT_TESTS_DIR}/../.." ABSOLUTE)

# Enables ARCANA_TEST_HOOKS globally (see the root CMakeLists), which the
# AppRuntime shutdown-race test needs in Core/AppRuntime's translation unit.
set(JSRUNTIMEHOST_TESTS ON CACHE BOOL "" FORCE)

add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/BabylonNative")

FetchContent_MakeAvailable_With_Message(googletest)

# Android runs the JsRuntimeHost (JS runtime / polyfill) half of the suite only.
# The graphics tests in Apps/UnitTests need a Babylon::Graphics::Device backed by
# a real surface, which an instrumented test has no window for; they stay desktop
# only for now (BABYLON_NATIVE_UNIT_TESTS_GRAPHICS).
add_library(UnitTestsJNI SHARED
    JNI.cpp
    ${UNIT_TESTS_DIR}/Source/Tests.JsRuntime.cpp)

target_compile_definitions(UnitTestsJNI
    PRIVATE ARCANA_TEST_HOOKS
    PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")

target_include_directories(UnitTestsJNI
    PRIVATE ${UNIT_TESTS_DIR}/Source)

target_link_libraries(UnitTestsJNI
    PRIVATE log
    PRIVATE AndroidExtensions
    PRIVATE AbortController
    PRIVATE AppRuntime
    PRIVATE Blob
    PRIVATE Console
    PRIVATE Fetch
    PRIVATE File
    PRIVATE Performance
    PRIVATE Scheduling
    PRIVATE ScriptLoader
    PRIVATE TextDecoder
    PRIVATE TextEncoder
    PRIVATE URL
    PRIVATE UrlLib
    PRIVATE WebSocket
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main)

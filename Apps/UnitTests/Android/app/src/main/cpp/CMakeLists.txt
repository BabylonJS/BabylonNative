cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UnitTestsJNI)

get_filename_component(UNIT_TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
get_filename_component(REPO_ROOT_DIR "${UNIT_TESTS_DIR}/../.." ABSOLUTE)

set(JSRUNTIMEHOST_TESTS OFF) # Turn off the tests folder for Android Studio path
add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/JsRuntimeHost")

npm(install "${UNIT_TESTS_DIR}/../" "UnitTests")

add_library(UnitTestsJNI SHARED
    JNI.cpp
    ${UNIT_TESTS_DIR}/Shared/Tests.h)
target_compile_definitions(UnitTestsJNI PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")

target_include_directories(UnitTestsJNI
    PRIVATE ${UNIT_TESTS_DIR})

target_link_libraries(UnitTestsJNI
    PRIVATE log
    PRIVATE AndroidExtensions
    PRIVATE AppRuntime
    PRIVATE AbortController
    PRIVATE Console
    PRIVATE Scheduling
    PRIVATE ScriptLoader
    PRIVATE URL
    PRIVATE UrlLib
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main
    PRIVATE WebSocket)

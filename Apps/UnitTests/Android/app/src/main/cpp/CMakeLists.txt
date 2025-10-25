cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UnitTestsJNI)

get_filename_component(UNIT_TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
get_filename_component(APPS_DIR "${UNIT_TESTS_DIR}/.." ABSOLUTE)
get_filename_component(REPO_ROOT_DIR "${APPS_DIR}/.." ABSOLUTE)

set(JSRUNTIMEHOST_TESTS OFF) # Turn off the tests folder for Android Studio path
add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/BabylonNative")

npm(install --silent WORKING_DIRECTORY ${APPS_DIR})

add_library(UnitTestsJNI SHARED
    JNI.cpp
    ${UNIT_TESTS_DIR}/Shared/Tests.h)
target_compile_definitions(UnitTestsJNI PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")

target_include_directories(UnitTestsJNI
    PRIVATE ${UNIT_TESTS_DIR})

target_link_libraries(UnitTestsJNI
    PRIVATE GLESv3
    PRIVATE android
    PRIVATE EGL
    PRIVATE log
    PRIVATE -lz
    PRIVATE AndroidExtensions
    PRIVATE AppRuntime
    PRIVATE Canvas
    PRIVATE Console
    PRIVATE GraphicsDevice
    PRIVATE NativeCamera
    PRIVATE NativeEngine
    PRIVATE NativeInput
    PRIVATE NativeOptimizations
    PRIVATE NativeEncoding
    PRIVATE ScriptLoader
    PRIVATE XMLHttpRequest
    PRIVATE gtest_main
    PRIVATE Window)

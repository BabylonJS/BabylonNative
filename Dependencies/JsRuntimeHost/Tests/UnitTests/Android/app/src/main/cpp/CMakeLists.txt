cmake_minimum_required(VERSION 3.18)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(UnitTestsJNI)

get_filename_component(UNIT_TESTS_DIR "${CMAKE_CURRENT_LIST_DIR}/../../../../.." ABSOLUTE)
get_filename_component(TESTS_DIR "${UNIT_TESTS_DIR}/.." ABSOLUTE)
get_filename_component(REPO_ROOT_DIR "${TESTS_DIR}/.." ABSOLUTE)

set(JSRUNTIMEHOST_TESTS ON CACHE BOOL "" FORCE)
add_subdirectory(${REPO_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/JsRuntimeHost")

FetchContent_MakeAvailable_With_Message(googletest)

npm(install --silent WORKING_DIRECTORY ${TESTS_DIR})

if(WIN32)
    set(NODE_LITE_PLATFORM_SRC ${TESTS_DIR}/NodeApi/node_lite_windows.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC ${TESTS_DIR}/NodeApi/child_process.cpp)
elseif(APPLE)
    set(NODE_LITE_PLATFORM_SRC ${TESTS_DIR}/NodeApi/node_lite_posix.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC ${TESTS_DIR}/NodeApi/child_process_posix.cpp)
elseif(ANDROID)
    set(NODE_LITE_PLATFORM_SRC ${TESTS_DIR}/NodeApi/node_lite_android.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC ${TESTS_DIR}/NodeApi/child_process_android.cpp)
else()
    set(NODE_LITE_PLATFORM_SRC ${TESTS_DIR}/NodeApi/node_lite_posix.cpp)
    set(NODE_LITE_CHILD_PROCESS_SRC ${TESTS_DIR}/NodeApi/child_process_posix.cpp)
endif()

add_library(UnitTestsJNI SHARED
    JNI.cpp
    ${UNIT_TESTS_DIR}/Shared/DelayedTaskScheduler.cpp
    ${UNIT_TESTS_DIR}/Shared/StandardStreamLogger.cpp
    ${UNIT_TESTS_DIR}/Shared/TimeoutDispatcher.cpp
    ${UNIT_TESTS_DIR}/Shared/Shared.h
    ${UNIT_TESTS_DIR}/Shared/Shared.cpp
    ${TESTS_DIR}/NodeApi/node_lite.cpp
    ${NODE_LITE_PLATFORM_SRC}
    ${NODE_LITE_CHILD_PROCESS_SRC}
    ${TESTS_DIR}/NodeApi/node_lite_jsruntimehost.cpp
    ${TESTS_DIR}/NodeApi/js_runtime_api.cpp
    ${TESTS_DIR}/NodeApi/string_utils.cpp
    ${TESTS_DIR}/NodeApi/test_main.cpp)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
    target_sources(UnitTestsJNI PRIVATE ${UNIT_TESTS_DIR}/Shared/V8ForegroundTaskRunner.cpp)
    target_link_libraries(UnitTestsJNI PRIVATE AppRuntimeInternal)
endif()

target_compile_definitions(UnitTestsJNI PRIVATE JSRUNTIMEHOST_PLATFORM="${JSRUNTIMEHOST_PLATFORM}")
target_compile_definitions(UnitTestsJNI PRIVATE NAPI_JAVASCRIPT_ENGINE="${NAPI_JAVASCRIPT_ENGINE}")
target_compile_definitions(UnitTestsJNI PRIVATE ARCANA_TEST_HOOKS)

target_include_directories(UnitTestsJNI
    PRIVATE ${UNIT_TESTS_DIR}
    PRIVATE ${TESTS_DIR}/NodeApi
    PRIVATE ${TESTS_DIR}/NodeApi/include
    PRIVATE ${REPO_ROOT_DIR}/Core/Node-API/Source)

target_link_libraries(UnitTestsJNI
    PRIVATE log
    PRIVATE AndroidExtensions
    PRIVATE AppRuntime
    PRIVATE AbortController
    PRIVATE Console
    PRIVATE SchedulingInternal
    PRIVATE FoundationInternal
    PRIVATE ScriptLoader
    PRIVATE URL
    PRIVATE UrlLib
    PRIVATE XMLHttpRequest
    PRIVATE Fetch
    PRIVATE WebSocket
    PRIVATE gtest_main
    PRIVATE Blob
    PRIVATE File
    PRIVATE TextDecoder
    PRIVATE TextEncoder
    PRIVATE Performance
    PRIVATE napi)

if(ANDROID)
    target_link_libraries(UnitTestsJNI PRIVATE dl)
endif()
set(JSR_NODE_API_NATIVE_TESTS 2_function_arguments 3_callbacks 4_object_factory 5_function_factory)
string(JOIN "," JSR_NODE_API_NATIVE_TESTS_CSV ${JSR_NODE_API_NATIVE_TESTS})
target_compile_definitions(UnitTestsJNI
    PRIVATE NODE_API_AVAILABLE_NATIVE_TESTS="${JSR_NODE_API_NATIVE_TESTS_CSV}")

# --- Statically linked Node-API conformance addons (Android in-process runner) ------------------
# Dynamic .node loading is never shipped to the Play / Quest stores, and bionic will not resolve a
# dlopen'd addon's napi_* imports against the System.loadLibrary-loaded host anyway (see
# NAPI_VERSION_ROADMAP.md; task #9 tracks the shared-lib alternative). So compile each addon into the
# host with a uniquely suffixed entry point -- avoiding one-definition clashes between addons -- and
# let node_lite resolve them from the in-process static registry (node_lite_android) instead of dlsym.
# entry_point.h emits, per addon, a load-time constructor that self-registers those entry points.
foreach(_mod ${JSR_NODE_API_NATIVE_TESTS})
    add_library(jsr_addon_${_mod} OBJECT
        ${TESTS_DIR}/NodeApi/test/js-native-api/${_mod}/${_mod}.c)
    target_include_directories(jsr_addon_${_mod} PRIVATE
        ${TESTS_DIR}/NodeApi/include
        ${TESTS_DIR}/NodeApi/test/js-native-api
        ${REPO_ROOT_DIR}/Core/Node-API/Include/Shared
        ${REPO_ROOT_DIR}/Core/Node-API/Include/Shared/napi
        ${REPO_ROOT_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}
        ${REPO_ROOT_DIR}/Core/Node-API/Include/Engine/${NAPI_JAVASCRIPT_ENGINE}/napi)
    target_compile_definitions(jsr_addon_${_mod} PRIVATE
        JSR_NODE_API_STATIC_LINK
        NODE_API_EXPERIMENTAL_NO_WARNING
        NODE_GYP_MODULE_NAME=\"${_mod}\"
        NODE_API_MODULE_REGISTER_FUNCTION=jsr_napi_register_${_mod}
        NODE_API_MODULE_GET_API_VERSION_FUNCTION=jsr_get_api_version_${_mod})
    target_sources(UnitTestsJNI PRIVATE $<TARGET_OBJECTS:jsr_addon_${_mod}>)
endforeach()

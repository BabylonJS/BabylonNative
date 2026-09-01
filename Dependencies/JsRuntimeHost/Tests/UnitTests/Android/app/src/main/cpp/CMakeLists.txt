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

# The in-process Node-API conformance harness only builds against engines that export the full C
# napi_* surface it uses. Hermes supplies napi through its own hermesNapi library, which does not
# export e.g. napi_create_external / napi_get_value_external, so linking it fails; QuickJS is likewise
# not validated on-device yet. Opt an engine in here once its harness run is green.
set(JSR_NODE_API_HARNESS_ENGINES V8 JavaScriptCore)
if(NAPI_JAVASCRIPT_ENGINE IN_LIST JSR_NODE_API_HARNESS_ENGINES)
    set(JSR_NODE_API_HARNESS_SRC
        ${TESTS_DIR}/NodeApi/node_lite.cpp
        ${NODE_LITE_PLATFORM_SRC}
        ${NODE_LITE_CHILD_PROCESS_SRC}
        ${TESTS_DIR}/NodeApi/node_lite_jsruntimehost.cpp
        ${TESTS_DIR}/NodeApi/js_runtime_api.cpp
        ${TESTS_DIR}/NodeApi/string_utils.cpp
        ${TESTS_DIR}/NodeApi/test_main.cpp)
endif()

add_library(UnitTestsJNI SHARED
    JNI.cpp
    ${UNIT_TESTS_DIR}/Shared/DelayedTaskScheduler.cpp
    ${UNIT_TESTS_DIR}/Shared/StandardStreamLogger.cpp
    ${UNIT_TESTS_DIR}/Shared/TimeoutDispatcher.cpp
    ${UNIT_TESTS_DIR}/Shared/Shared.h
    ${UNIT_TESTS_DIR}/Shared/Shared.cpp
    ${JSR_NODE_API_HARNESS_SRC})

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
if(JSR_NODE_API_HARNESS_SRC)
    target_compile_definitions(UnitTestsJNI
        PRIVATE NODE_API_AVAILABLE_NATIVE_TESTS="2_function_arguments,3_callbacks,4_object_factory,5_function_factory,test_reference_double_free")
endif()

# The conformance addons are built as standalone SHARED lib<name>.so by Tests/NodeApi/CMakeLists.txt,
# packaged by AGP into nativeLibraryDir, and dlopen'd in-process by node_lite_android. Both they and
# this host link the shared napi (libnapi.so), so the addons' napi_* imports resolve at load.

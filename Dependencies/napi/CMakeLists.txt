cmake_minimum_required(VERSION 3.12)

project(napi)

# Set per-platform defaults if no JS engine is set.
if (WIN32)
    set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for N-API.")
elseif (ANDROID)
    # engine is selected in native build cmakelists.txt
elseif (APPLE)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for N-API.")
elseif (UNIX)
    set(NAPI_JAVASCRIPT_ENGINE "V8" CACHE STRING "JavaScript engine for N-API.")
else()
    message(FATAL_ERROR "Unable to select N-API JavaScript engine for platform.")
endif()

# Check that the requested JavaScript engine is supported.
set(SUPPORTED_JAVASCRIPT_ENGINES "Chakra" "V8" "JavaScriptCore" "JSI")
if(NOT NAPI_JAVASCRIPT_ENGINE IN_LIST SUPPORTED_JAVASCRIPT_ENGINES)
    message(FATAL_ERROR "Unsupported engine: ${NAPI_JAVASCRIPT_ENGINE}")
endif()

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    add_subdirectory(napi-jsi)
else()
    add_subdirectory(napi-direct)
endif()

add_on_linked_as_dependency_cmake_file(napi "${CMAKE_CURRENT_SOURCE_DIR}/OnLinkedAsDependency.cmake")
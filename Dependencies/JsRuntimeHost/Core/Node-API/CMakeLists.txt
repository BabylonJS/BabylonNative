option(NAPI_BUILD_ABI "Build the ABI layer." ON)

set(SOURCES
    "include/napi/env.h"
    "include/napi/js_native_api.h"
    "include/napi/js_native_api_types.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "source/env.cc")

if(NAPI_BUILD_ABI)
    # Set per-platform defaults if unspecified.
    if(WIN32)
        set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for node-api-env")
    elseif(APPLE)
        set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for node-api-env")
    elseif(UNIX)
        set(NAPI_JAVASCRIPT_ENGINE "V8" CACHE STRING "JavaScript engine for node-api-env")
    else()
        message(FATAL_ERROR "Unable to select node-api-env JavaScript engine for platform")
    endif()

    if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Chakra")
        set(SOURCES ${SOURCES}
            "source/env_chakra.cc"
            "source/js_native_api_chakra.cc"
            "source/js_native_api_chakra.h")

        set(LINK_LIBRARIES
            INTERFACE "chakrart.lib")
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "JavaScriptCore")
        set(SOURCES ${SOURCES}
            "source/env_javascriptcore.cc"
            "source/js_native_api_javascriptcore.cc"
            "source/js_native_api_javascriptcore.h")
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
        set(SOURCES ${SOURCES}
            "source/env_v8.cc"
            "source/js_native_api_v8.cc"
            "source/js_native_api_v8.h"
            "source/js_native_api_v8_internals.h")
    elseif()
        message(FATAL_ERROR "Unsupported JavaScript engine: ${NAPI_JAVASCRIPT_ENGINE}")
    endif()

    message(STATUS "Selected ${NAPI_JAVASCRIPT_ENGINE}")
endif()

add_library(napi ${SOURCES})

target_compile_definitions(napi
    PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED
    PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC
    ${COMPILE_DEFINITIONS})

target_include_directories(napi PUBLIC "include")

target_link_libraries(napi
    ${LINK_LIBRARIES})

set_property(TARGET napi PROPERTY FOLDER Dependencies)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

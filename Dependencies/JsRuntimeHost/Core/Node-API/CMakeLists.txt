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
        set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for Node-API")
    elseif(APPLE)
        set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
    elseif(UNIX)
        set(NAPI_JAVASCRIPT_ENGINE "V8" CACHE STRING "JavaScript engine for Node-API")
    else()
        message(FATAL_ERROR "Unable to select Node-API JavaScript engine for platform")
    endif()

    if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Chakra")
        set(SOURCES ${SOURCES}
            "source/env_chakra.cc"
            "source/js_native_api_chakra.cc"
            "source/js_native_api_chakra.h")

        set(ADDITIONAL_LINK_LIBRARIES
            INTERFACE "chakrart.lib")
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "JavaScriptCore")
        set(SOURCES ${SOURCES}
            "source/env_javascriptcore.cc"
            "source/js_native_api_javascriptcore.cc"
            "source/js_native_api_javascriptcore.h")

        if(ANDROID)
            message(STATUS "Installing jsc-android module")
            file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/package-jsc.json" "${CMAKE_CURRENT_BINARY_DIR}/package.json" SYMBOLIC)
            execute_process(COMMAND ${NPM_COMMAND} install --silent --no-package-lock WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE NPM_RESULT)
            if(NPM_RESULT EQUAL 0)
                message(STATUS "Installing jsc-android module - done")
            else()
                message(FATAL_ERROR "npm command failed: ${NPM_RESULT}")
            endif()

            file(GLOB_RECURSE JSC_ANDROID_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/node_modules/jsc-android/dist/org/webkit/android-jsc/*.aar")
            set(JSC_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/jsc-android")

            message(STATUS "Extracting jsc-android archive")
            file(ARCHIVE_EXTRACT INPUT ${JSC_ANDROID_ARCHIVE} DESTINATION ${JSC_ANDROID_DIR} PATTERNS jni)
            message(STATUS "Extracting jsc-android archive - done")

            file(MAKE_DIRECTORY "${JSC_ANDROID_DIR}/include")
            file(CREATE_LINK "${CMAKE_CURRENT_BINARY_DIR}/node_modules/jsc-android/dist/include" "${JSC_ANDROID_DIR}/include/JavaScriptCore" SYMBOLIC)

            set(ADDITIONAL_INCLUDES
                PUBLIC "${JSC_ANDROID_DIR}/include")

            set(ADDITIONAL_LINK_LIBRARIES
                PUBLIC "${JSC_ANDROID_DIR}/jni/${ANDROID_ABI}/libjsc.so")
        elseif(APPLE)
            find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
            set(ADDITIONAL_LINK_LIBRARIES
                PUBLIC ${JAVASCRIPTCORE_LIBRARY})
        endif()
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

target_include_directories(napi
    PUBLIC "include"
    ${ADDITIONAL_INCLUDES})

target_link_libraries(napi
    ${ADDITIONAL_LINK_LIBRARIES})

set_property(TARGET napi PROPERTY FOLDER Dependencies)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

option(NAPI_BUILD_ABI "Build the ABI layer." ON)

set(SOURCES
    "include/napi/env.h"
    "include/napi/js_native_api.h"
    "include/napi/js_native_api_types.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "source/env.cc")

function(napi_install_android_package name aar_path output_directory)
    message(STATUS "Installing ${name}-android module")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/package-${name}.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
    file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/package-${name}.json" "${CMAKE_CURRENT_BINARY_DIR}/package.json")
    execute_process(COMMAND ${NPM_COMMAND} install --silent --no-package-lock WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR} RESULT_VARIABLE NPM_RESULT)
    if(NPM_RESULT EQUAL 0)
        message(STATUS "Installing ${name}-android module - done")
    else()
        message(FATAL_ERROR "npm command failed: ${NPM_RESULT}")
    endif()

    file(GLOB_RECURSE ANDROID_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/node_modules/${name}-android/${aar_path}/*.aar")
    file(ARCHIVE_EXTRACT INPUT ${ANDROID_ARCHIVE} DESTINATION ${output_directory} PATTERNS jni)
    message(STATUS "Extracting ${name}-android archive - done")

    file(COPY "${CMAKE_CURRENT_BINARY_DIR}/node_modules/${name}-android/dist/include" DESTINATION ${output_directory})
endfunction()

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
            set(JSC_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/jsc-android")
            napi_install_android_package(jsc "dist/org/webkit/android-jsc" ${JSC_ANDROID_DIR})

            # Add `JavaScriptCore` prefix to the include path
            file(RENAME "${JSC_ANDROID_DIR}/include" "${JSC_ANDROID_DIR}/JavaScriptCore")
            file(MAKE_DIRECTORY "${JSC_ANDROID_DIR}/include")
            file(RENAME "${JSC_ANDROID_DIR}/JavaScriptCore" "${JSC_ANDROID_DIR}/include/JavaScriptCore")

            set(ADDITIONAL_INCLUDES
                PUBLIC "${JSC_ANDROID_DIR}/include")

            set(ADDITIONAL_LINK_LIBRARIES
                PUBLIC "${JSC_ANDROID_DIR}/jni/${ANDROID_ABI}/libjsc.so")
        elseif(APPLE)
            find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
            set(ADDITIONAL_LINK_LIBRARIES
                PUBLIC ${JAVASCRIPTCORE_LIBRARY})
        else()
            message(FATAL_ERROR "Unsupported JavaScript engine: ${NAPI_JAVASCRIPT_ENGINE}")
        endif()
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
        set(SOURCES ${SOURCES}
            "source/env_v8.cc"
            "source/js_native_api_v8.cc"
            "source/js_native_api_v8.h"
            "source/js_native_api_v8_internals.h")

        if(ANDROID)
            set(V8_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/v8-android")
            napi_install_android_package(v8 "dist/org/chromium" ${V8_ANDROID_DIR})

            set(ADDITIONAL_INCLUDES
                PUBLIC "${V8_ANDROID_DIR}/include")

            set(ADDITIONAL_LINK_LIBRARIES
                PUBLIC "${V8_ANDROID_DIR}/jni/${ANDROID_ABI}/libv8android.so")
        else()
            message(FATAL_ERROR "Unsupported JavaScript engine: ${NAPI_JAVASCRIPT_ENGINE}")
        endif()
    else()
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

# Set per-platform defaults if unspecified.
if(WIN32)
    set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for Node-API")
elseif(APPLE)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
elseif(ANDROID)
    set(NAPI_JAVASCRIPT_ENGINE "V8" CACHE STRING "JavaScript engine for Node-API")
elseif(UNIX)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
    set(JAVASCRIPTCORE_LIBRARY "/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.0.so" CACHE STRING "Path to the JavaScriptCore shared library")
else()
    message(FATAL_ERROR "Unable to select Node-API JavaScript engine for platform")
endif()

set(SOURCES
    "Include/Engine/${NAPI_JAVASCRIPT_ENGINE}/napi/env.h"
    "Include/Shared/napi/js_native_api.h"
    "Include/Shared/napi/js_native_api_types.h"
    "Include/Shared/napi/napi.h"
    "Include/Shared/napi/napi-inl.h"
    "Source/env.cc")

set(INCLUDE_DIRECTORIES
    PUBLIC "Include/Shared"
    PUBLIC "Include/Engine/${NAPI_JAVASCRIPT_ENGINE}")

if(NAPI_BUILD_ABI)
    if(ANDROID)
        function(napi_install_android_package name aar_path output_directory)
            file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/package-${name}.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
            file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/package-${name}.json" "${CMAKE_CURRENT_BINARY_DIR}/package.json")
            npm(install --no-package-lock --silent WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

            file(GLOB_RECURSE ANDROID_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/node_modules/${name}-android/${aar_path}/*.aar")
            file(ARCHIVE_EXTRACT INPUT ${ANDROID_ARCHIVE} DESTINATION ${output_directory} PATTERNS jni)
            message(STATUS "Extracting ${name}-android archive - done")

            file(COPY "${CMAKE_CURRENT_BINARY_DIR}/node_modules/${name}-android/dist/include" DESTINATION ${output_directory})
        endfunction()
    endif()

    if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Chakra")
        set(SOURCES ${SOURCES}
            "Source/env_chakra.cc"
            "Source/js_native_api_chakra.cc"
            "Source/js_native_api_chakra.h")

        set(LINK_LIBRARIES ${LINK_LIBRARIES}
            INTERFACE "chakrart.lib")
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "JavaScriptCore")
        set(SOURCES ${SOURCES}
            "Source/env_javascriptcore.cc"
            "Source/js_native_api_javascriptcore.cc"
            "Source/js_native_api_javascriptcore.h")

        if(ANDROID)
            set(JSC_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/jsc-android")
            napi_install_android_package(jsc "dist/org/webkit/android-jsc" ${JSC_ANDROID_DIR})

            # Add `JavaScriptCore` prefix to the include path
            file(RENAME "${JSC_ANDROID_DIR}/include" "${JSC_ANDROID_DIR}/JavaScriptCore")
            file(MAKE_DIRECTORY "${JSC_ANDROID_DIR}/include")
            file(RENAME "${JSC_ANDROID_DIR}/JavaScriptCore" "${JSC_ANDROID_DIR}/include/JavaScriptCore")

            set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
                PUBLIC "${JSC_ANDROID_DIR}/include")

            set(LINK_LIBRARIES ${LINK_LIBRARIES}
                PUBLIC "${JSC_ANDROID_DIR}/jni/${ANDROID_ABI}/libjsc.so")
        elseif(APPLE)
            find_library(JAVASCRIPTCORE_LIBRARY JavaScriptCore)
            set(LINK_LIBRARIES ${LINK_LIBRARIES}
                PUBLIC ${JAVASCRIPTCORE_LIBRARY})
        elseif(UNIX)
            set(LINK_LIBRARIES ${LINK_LIBRARIES}
                PUBLIC ${JAVASCRIPTCORE_LIBRARY})
            set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
                PUBLIC "/usr/include/webkitgtk-4.0/")
        else()
            message(FATAL_ERROR "Unsupported JavaScript engine: ${NAPI_JAVASCRIPT_ENGINE}")
        endif()
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
        set(SOURCES ${SOURCES}
            "Source/env_v8.cc"
            "Source/js_native_api_v8.cc"
            "Source/js_native_api_v8.h"
            "Source/js_native_api_v8_internals.h")

        if(ANDROID)
            set(V8_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/v8-android")
            napi_install_android_package(v8 "dist/org/chromium" ${V8_ANDROID_DIR})

            set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
                PUBLIC "${V8_ANDROID_DIR}/include")

            set(LINK_LIBRARIES ${LINK_LIBRARIES}
                PUBLIC "${V8_ANDROID_DIR}/jni/${ANDROID_ABI}/libv8android.so")
        elseif(WIN32)
            set_cpu_platform_arch()
            set(V8_VERSION "8.4.371.15")
            download_nuget()
            set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v142-${CPU_ARCH}.${V8_VERSION}")
            set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v142-${CPU_ARCH}.${V8_VERSION}")
            set(V8_REDIST_PACKAGE_PATH "${NUGET_PATH}/packages/v8.redist-v142-${CPU_ARCH}.${V8_VERSION}")

            add_library(v8_libbase SHARED IMPORTED)
            set_target_properties(v8_libbase PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libbase.dll.lib")
            add_library(v8_libplatform SHARED IMPORTED)
            set_target_properties(v8_libplatform PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libplatform.dll.lib")
            add_library(v8 SHARED IMPORTED)
            set_target_properties(v8 PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8.dll.lib")
            target_link_libraries(v8 INTERFACE v8_libbase INTERFACE v8_libplatform)
            target_include_directories(v8 INTERFACE "${V8_PACKAGE_PATH}/include")
            
            set(V8_DIST
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/icudtl.dat"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/icui18n.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/icuuc.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libbase.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libplatform.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/zlib.dll")

            # only 1 imported location per library -> Adding 1 library per file
            foreach(V8FILE ${V8_DIST})
                get_filename_component(V8FILE_NAME "${V8FILE}" NAME_WE)
                add_library("v8::${V8FILE_NAME}" SHARED IMPORTED)
                set_target_properties("v8::${V8FILE_NAME}" PROPERTIES IMPORTED_IMPLIB "${V8_PACKAGE_PATH}/lib/Release/v8_libbase.dll.lib")
                set_target_properties("v8::${V8FILE_NAME}" PROPERTIES IMPORTED_LOCATION ${V8FILE})
            endforeach()

            set(LINK_LIBRARIES ${LINK_LIBRARIES}
                PUBLIC v8
                PRIVATE v8::icudtl
                PRIVATE v8::icui18n
                PRIVATE v8::icuuc
                PRIVATE v8::v8
                PRIVATE v8::v8_libbase
                PRIVATE v8::v8_libplatform
                PRIVATE v8::zlib)
            
            if(CPU_ARCH STREQUAL "x64")
                # Enable V8 Pointer Compression
                # https://v8.dev/blog/pointer-compression
                # https://stackoverflow.com/q/62921373
                set(NAPI_DEFINITIONS PUBLIC V8_COMPRESS_POINTERS)
            endif()
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
    ${NAPI_DEFINITIONS})

target_include_directories(napi ${INCLUDE_DIRECTORIES})
target_link_libraries(napi ${LINK_LIBRARIES})

set_property(TARGET napi PROPERTY FOLDER Dependencies)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

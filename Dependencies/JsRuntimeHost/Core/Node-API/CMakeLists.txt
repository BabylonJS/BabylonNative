# Set per-platform defaults if unspecified.
if(WIN32)
    set(NAPI_JAVASCRIPT_ENGINE "Chakra" CACHE STRING "JavaScript engine for Node-API")
elseif(APPLE)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
elseif(ANDROID)
    set(NAPI_JAVASCRIPT_ENGINE "V8" CACHE STRING "JavaScript engine for Node-API")
elseif(UNIX)
    set(NAPI_JAVASCRIPT_ENGINE "JavaScriptCore" CACHE STRING "JavaScript engine for Node-API")
    set(JAVASCRIPTCORE_LIBRARY "/usr/lib/x86_64-linux-gnu/libjavascriptcoregtk-4.1.so" CACHE STRING "Path to the JavaScriptCore shared library")
else()
    message(FATAL_ERROR "Unable to select Node-API JavaScript engine for platform")
endif()

set(SOURCES
    "Include/Engine/${NAPI_JAVASCRIPT_ENGINE}/napi/env.h"
    "Include/Shared/napi/js_native_api.h"
    "Include/Shared/napi/js_native_api_types.h"
    "Include/Shared/napi/napi.h"
    "Include/Shared/napi/napi-inl.h")

# env.cc contains a generic `Napi::Eval` that goes through the C++ wrapper's
# `Env::RunScript`, which calls our 4-argument `napi_run_script` (Babylon
# extension carrying a `source_url`).  Hermes only provides the standard
# 3-argument `napi_run_script`, so for Hermes we skip env.cc and provide a
# Hermes-native `Napi::Eval` in env_hermes.cc that calls `hermes_run_script`.
if(NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "Hermes")
    list(APPEND SOURCES "Source/env.cc")
endif()

set(INCLUDE_DIRECTORIES
    PUBLIC "Include/Shared"
    PUBLIC "Include/Engine/${NAPI_JAVASCRIPT_ENGINE}")

if(NAPI_BUILD_ABI)
    if(ANDROID)
        function(napi_install_android_package name aar_path output_directory)
            file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/package-${name}.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
            file(RENAME "${CMAKE_CURRENT_BINARY_DIR}/package-${name}.json" "${CMAKE_CURRENT_BINARY_DIR}/package.json")
            npm(install --no-package-lock --silent WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

            file(GLOB_RECURSE ANDROID_ARCHIVE "${CMAKE_CURRENT_BINARY_DIR}/node_modules/${V8_PACKAGE_NAME}/${aar_path}/*.aar")
            file(ARCHIVE_EXTRACT INPUT ${ANDROID_ARCHIVE} DESTINATION ${output_directory} PATTERNS jni)
            message(STATUS "Extracting ${V8_PACKAGE_NAME} archive - done")

            file(COPY "${CMAKE_CURRENT_BINARY_DIR}/node_modules/${V8_PACKAGE_NAME}/dist/include" DESTINATION ${output_directory})
        endfunction()
    endif()

    if(NAPI_JAVASCRIPT_ENGINE STREQUAL "QuickJS")
        set(SOURCES ${SOURCES}
            "Source/env_quickjs.cc"
            "Source/js_native_api_quickjs.cc"
            "Source/js_native_api_quickjs.h")
        set(LINK_LIBRARIES ${LINK_LIBRARIES} PUBLIC qjs)
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "Chakra")
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
            set(V8_PACKAGE_NAME "jsc-android")
            set(JSC_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/${V8_PACKAGE_NAME}")
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
                PUBLIC "/usr/include/webkitgtk-4.1/")
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
            set(V8_PACKAGE_BASE_NAME "v8-android-jit-nointl-nosnapshot")
            set(V8_PACKAGE_NAME "@babylonjs/${V8_PACKAGE_BASE_NAME}")
            set(V8_ANDROID_DIR "${CMAKE_CURRENT_BINARY_DIR}/${V8_PACKAGE_BASE_NAME}")
            napi_install_android_package(v8 "dist/org/chromium" ${V8_ANDROID_DIR})

            set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
                PUBLIC "${V8_ANDROID_DIR}/include")

            set(LINK_LIBRARIES ${LINK_LIBRARIES}
                PUBLIC "${V8_ANDROID_DIR}/jni/${ANDROID_ABI}/libv8android.so")
        elseif(WIN32)
            set_cpu_platform_arch()
            set(V8_VERSION "11.9.169.4")
            download_nuget()
            set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v143-${CPU_ARCH}.${V8_VERSION}")
            set(V8_PACKAGE_PATH "${NUGET_PATH}/packages/v8-v143-${CPU_ARCH}.${V8_VERSION}")
            set(V8_REDIST_PACKAGE_PATH "${NUGET_PATH}/packages/v8.redist-v143-${CPU_ARCH}.${V8_VERSION}")

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
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/third_party_icu_icui18n.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/third_party_abseil-cpp_absl.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/icuuc.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libbase.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/v8_libplatform.dll"
                "${V8_REDIST_PACKAGE_PATH}/lib/Release/third_party_zlib.dll")

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
                PRIVATE v8::third_party_icu_icui18n
                PRIVATE v8::icuuc
                PRIVATE v8::v8
                PRIVATE v8::v8_libbase
                PRIVATE v8::v8_libplatform
                PRIVATE v8::third_party_zlib)
        else()
            message(FATAL_ERROR "Unsupported JavaScript engine: ${NAPI_JAVASCRIPT_ENGINE}")
        endif()
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "Hermes")
        # Hermes provides its own implementation of the standard Node-API C
        # functions inside the `hermesNapi` static library, so we don't ship a
        # `js_native_api_hermes.cc` here.  We only contribute the Babylon-side
        # bridge (`env_hermes.cc`) which adapts our `Napi::Attach`/`Detach`/
        # `Eval` helpers to Hermes's `hermes_napi_create_env` /
        # `hermes_run_script` APIs.
        set(SOURCES ${SOURCES}
            "Source/env_hermes.cc")

        if(NOT TARGET hermesNapi)
            message(FATAL_ERROR "NAPI_JAVASCRIPT_ENGINE=Hermes requires the hermesNapi target. \
Make sure Hermes was fetched at the top-level CMakeLists.txt and NAPI_JAVASCRIPT_ENGINE was set BEFORE configure.")
        endif()

        # Link Hermes PRIVATEly so its include directories and compile
        # definitions (notably `NAPI_VERSION=10` and the vendored
        # `hermes/napi/*` headers) don't leak to consumers of `napi`.
        # Consumers continue to see only our shared `napi/*` headers.
        #
        # We link against Hermes's umbrella static library `hermesvm_a` (which
        # transitively pulls in all the BCGen/Optimizer/Regex/Support/etc.
        # object libraries that `hermesVMRuntime` depends on) plus the NAPI
        # static library on top.  Linking just `hermesNapi + hermesVMRuntime +
        # hermesPublic` leaves a long tail of unresolved symbols
        # (bigint::exponentiate, regex::parseRegex, hbc::compileEvalModule,
        # llvh::raw_ostream, platform_unicode::normalize, ...) which
        # `hermesvm_a` bundles together for us.
        set(LINK_LIBRARIES ${LINK_LIBRARIES}
            PRIVATE hermesNapi
            PRIVATE hermesvm_a)

        # `hermes_napi.h` lives in Hermes's `API/napi/` source directory and
        # is NOT installed alongside the other `include/hermes/napi/`
        # headers, so the hermesNapi PUBLIC include dir doesn't expose it.
        # Add the API/napi source directory explicitly so env_hermes.cc can
        # `#include "hermes_napi.h"`.
        #
        # Additionally, Hermes uses *global* include_directories() at its
        # project root (for the vendored LLVH headers and Hermes config
        # output) instead of attaching them to each target.  Those globals
        # don't propagate out of Hermes's add_subdirectory scope, so any
        # consumer (including our napi target) that pulls in Hermes headers
        # transitively needs them re-declared here.
        #
        # We resolve the binary directory via CMAKE_BINARY_DIR rather than
        # ${hermes_BINARY_DIR} because FetchContent's lowercased per-content
        # binary variable isn't always populated reliably across CMake
        # versions/scopes, whereas the standard `_deps/<name>-build` layout
        # is.
        set(_HERMES_BINARY_DIR "${CMAKE_BINARY_DIR}/_deps/hermes-build")
        set(INCLUDE_DIRECTORIES ${INCLUDE_DIRECTORIES}
            PRIVATE "${hermes_SOURCE_DIR}/API/napi"
            PRIVATE "${hermes_SOURCE_DIR}/include"
            PRIVATE "${hermes_SOURCE_DIR}/public"
            PRIVATE "${hermes_SOURCE_DIR}/external/llvh/include"
            PRIVATE "${hermes_SOURCE_DIR}/external/llvh/gen/include"
            PRIVATE "${_HERMES_BINARY_DIR}/include"
            PRIVATE "${_HERMES_BINARY_DIR}/lib/config"
            PRIVATE "${_HERMES_BINARY_DIR}/external/llvh/include")

        # Hermes's headers rely on a handful of compiler-specific warning
        # suppressions that Hermes applies globally in its own
        # `Hermes.cmake` (e.g. `-wd4576` for C-style compound literals like
        # `(SHLegacyValue){...}` inside `sh_legacy_value.h`).  Those
        # globals don't propagate to our `napi` target, so we apply the
        # minimum set needed for the headers we transitively pull in via
        # `env_hermes.cc`.  These flags are MSVC-only — clang and gcc
        # accept compound literals as a GNU extension under `-std=gnu++20`
        # (which CMake picks by default) and would otherwise interpret a
        # `/wdNNNN` argument as a missing source file path.
        if(MSVC)
            set(NAPI_HERMES_COMPILE_OPTIONS
                /wd4068  # unknown pragma (GCC pragmas in Hermes headers)
                /wd4141  # 'inline': used more than once
                /wd4146  # unary minus on unsigned type
                /wd4244  # conversion possible loss of data
                /wd4267  # size_t -> smaller type conversion
                /wd4291  # no matching delete found
                /wd4576) # parenthesized type followed by initializer list
        else()
            set(NAPI_HERMES_COMPILE_OPTIONS "")
        endif()

        # Maximum GC heap size, in MiB, for the Hermes Runtime created by
        # `Napi::Attach` (see Source/env_hermes.cc).  This is the GC's upper
        # bound -- a ceiling, not an up-front reservation.  The 512 MiB default
        # gives the unit-test Mocha suite generous headroom over Hermes's tiny
        # built-in default; embedders running heavier workloads (large Babylon
        # scenes, big typed-array buffers) can raise it at configure time with:
        #     -DNAPI_HERMES_MAX_HEAP_SIZE_MB=<value>
        # The value is forwarded to env_hermes.cc as a compile definition of the
        # same name.
        set(NAPI_HERMES_MAX_HEAP_SIZE_MB 512 CACHE STRING
            "Maximum Hermes GC heap size in MiB for the Napi::Attach runtime (default 512)")
    else()
        message(FATAL_ERROR "Unsupported JavaScript engine: ${NAPI_JAVASCRIPT_ENGINE}")
    endif()

    message(STATUS "Selected ${NAPI_JAVASCRIPT_ENGINE}")
endif()

add_library(napi ${SOURCES})

target_include_directories(napi ${INCLUDE_DIRECTORIES})
target_link_libraries(napi ${LINK_LIBRARIES})

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Hermes")
    # Apply Hermes-specific warning suppressions ONLY to env_hermes.cc so
    # they don't relax the rules for the rest of the napi sources.
    set_source_files_properties("Source/env_hermes.cc"
        PROPERTIES COMPILE_OPTIONS "${NAPI_HERMES_COMPILE_OPTIONS}")

    # Forward the configurable Hermes GC max heap size to env_hermes.cc.
    target_compile_definitions(napi PRIVATE
        NAPI_HERMES_MAX_HEAP_SIZE_MB=${NAPI_HERMES_MAX_HEAP_SIZE_MB})
endif()

set_property(TARGET napi PROPERTY FOLDER Dependencies)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

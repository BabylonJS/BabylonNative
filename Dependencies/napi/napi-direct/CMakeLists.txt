STRING(TOLOWER ${NAPI_JAVASCRIPT_ENGINE} ENGINE_FILE_POSTFIX)

set(SOURCES
    "include/napi/env.h"
    "include/napi/js_native_api.h"
    "include/napi/js_native_api_types.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "source/env.cc"
    "source/env_${ENGINE_FILE_POSTFIX}.cc"
    "source/js_native_api_${ENGINE_FILE_POSTFIX}.cc"
    "source/js_native_api_${ENGINE_FILE_POSTFIX}.h"
    "source/js_native_api_${ENGINE_FILE_POSTFIX}_internals.h")

add_library(napi ${SOURCES})

target_compile_definitions(napi PRIVATE NOMINMAX)
target_compile_definitions(napi PRIVATE _CRT_SECURE_NO_WARNINGS)

target_compile_definitions(napi PUBLIC NODE_ADDON_API_DISABLE_DEPRECATED)
target_compile_definitions(napi PUBLIC NODE_ADDON_API_DISABLE_NODE_SPECIFIC)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "Chakra")
    target_compile_definitions(napi PUBLIC USE_EDGEMODE_JSRT)
endif()

target_include_directories(napi PUBLIC "include")

# Install v8 SDK from NuGet
function (install_v8_nuget V8_VERSION ARCH VS_PLAT_TOOLSET)
    set(NUGET_PATH "${CMAKE_BINARY_DIR}/NuGet")
    set(NUGET_EXE "${NUGET_PATH}/nuget.exe")
    if(NOT EXISTS ${NUGET_EXE})
        file(DOWNLOAD "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe" ${NUGET_EXE})
    endif()
    file(COPY "packages.config" DESTINATION ${NUGET_PATH})
    execute_process(COMMAND ${NUGET_EXE} install WORKING_DIRECTORY ${NUGET_PATH})
    add_library(v8_libbase SHARED IMPORTED)
    set_target_properties(v8_libbase PROPERTIES IMPORTED_IMPLIB "${NUGET_PATH}/v8-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/v8_libbase.dll.lib")
    add_library(v8_libplatform SHARED IMPORTED)
    set_target_properties(v8_libplatform PROPERTIES IMPORTED_IMPLIB "${NUGET_PATH}/v8-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/v8_libplatform.dll.lib")
    add_library(v8 SHARED IMPORTED)
    set_target_properties(v8 PROPERTIES IMPORTED_IMPLIB "${NUGET_PATH}/v8-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/v8.dll.lib")
    target_link_libraries(v8 INTERFACE v8_libbase INTERFACE v8_libplatform)
    target_include_directories(v8 INTERFACE "${NUGET_PATH}/v8-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/include")

    set(NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/icudtl.dat"
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/icui18n.dll"
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/icuuc.dll"
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/v8.dll"
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/v8_libbase.dll"
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/v8_libplatform.dll"
        "${NUGET_PATH}/v8.redist-${VS_PLAT_TOOLSET}-${ARCH}.${V8_VERSION}/lib/Release/zlib.dll"
        CACHE STRING "N-API runtime output artifacts")
endfunction()

if(NOT TARGET javascript_engine)
    add_library(javascript_engine INTERFACE)
    add_on_linked_as_dependency_cmake_file(napi "${CMAKE_CURRENT_SOURCE_DIR}/OnLinkedAsDependency.cmake")
    if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
        if(WIN32)
            set_win32_arch()        
            set(V8_VERSION "8.4.371.15")
            install_v8_nuget(${V8_VERSION} ${WIN32_ARCH} ${CMAKE_VS_PLATFORM_TOOLSET})
            target_link_to_dependencies(javascript_engine INTERFACE v8)

            if (WIN32_ARCH STREQUAL "x64")
                # Enable V8 Pointer Compression
                # https://v8.dev/blog/pointer-compression
                # https://stackoverflow.com/q/62921373
                target_compile_definitions(javascript_engine INTERFACE V8_COMPRESS_POINTERS)
            endif()
        elseif(UNIX AND NOT ANDROID AND NOT APPLE)
            target_include_directories(napi PUBLIC "/usr/include/v8")
            find_library(V8_LIBRARY v8)
            target_link_to_dependencies(javascript_engine INTERFACE ${V8_LIBRARY})
        endif()
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "Chakra")
        target_link_to_dependencies(javascript_engine INTERFACE "chakrart.lib")
    
    elseif(NAPI_JAVASCRIPT_ENGINE STREQUAL "JavaScriptCore")
        if(UNIX AND NOT ANDROID AND NOT APPLE)
            target_include_directories(napi PUBLIC "/usr/include/webkitgtk-4.0")
            find_library(JSCORE_LIBRARY JavaScriptCore)
            target_link_to_dependencies(javascript_engine INTERFACE ${JSCORE_LIBRARY})
        endif()
    endif()
endif()
target_link_to_dependencies(napi PUBLIC javascript_engine)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

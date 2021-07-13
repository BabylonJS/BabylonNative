set(SOURCES
    "include/napi/env.h"
    "include/napi/napi.h"
    "include/napi/napi-inl.h"
    "include/napi/js_native_api_types.h"
    "include/napi/js_native_api.h"
    "source/env.cc")

if(NOT TARGET jsi)
    if(WIN32)
        nuget_download()
        set_win32_arch()
        set(V8JSI_VERSION "0.64.16")
        if (WINDOWS_STORE)
            set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.UWP.${V8JSI_VERSION}")
            set(PLATFORM_FOLDER "uwp")
        else()
            set(V8JSI_PACKAGE_PATH "${NUGET_PATH}/packages/ReactNative.V8Jsi.Windows.${V8JSI_VERSION}")
            set(PLATFORM_FOLDER "win32")
        endif()

        # TODO: Pull in v8jsi symbols once they're packaged with the debug build.
        set(V8JSI_LIB_PATH_DEBUG "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Debug/${WIN32_ARCH}/v8jsi.dll.lib")
        set(V8JSI_LIB_PATH_RELEASE "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Release/${WIN32_ARCH}/v8jsi.dll.lib")
        set(V8JSI_DLL_PATH "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/$<IF:$<CONFIG:Debug>,Debug,Release>/${WIN32_ARCH}/v8jsi.dll")
        set(V8JSI_DLL_PATH_DEBUG "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Debug/${WIN32_ARCH}/v8jsi.dll")
        set(V8JSI_DLL_PATH_RELEASE "${V8JSI_PACKAGE_PATH}/lib/${PLATFORM_FOLDER}/Release/${WIN32_ARCH}/v8jsi.dll")

        add_library(jsi SHARED IMPORTED)
        set_target_properties(jsi PROPERTIES 
            IMPORTED_IMPLIB_DEBUG ${V8JSI_LIB_PATH_DEBUG}
            IMPORTED_IMPLIB_RELEASE ${V8JSI_LIB_PATH_RELEASE}
            IMPORTED_IMPLIB_MINSIZEREL ${V8JSI_LIB_PATH_RELEASE}
            IMPORTED_IMPLIB_RELWITHDEBINFO ${V8JSI_LIB_PATH_RELEASE})

        target_include_directories(jsi INTERFACE "${V8JSI_PACKAGE_PATH}/build/native/include")
        target_include_directories(jsi INTERFACE "${V8JSI_PACKAGE_PATH}/build/native/jsi")
        set(NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS_DEBUG ${V8JSI_DLL_PATH_DEBUG} CACHE STRING "N-API runtime output artifacts (debug)")
        set(NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS_RELEASE ${V8JSI_DLL_PATH_RELEASE} CACHE STRING "N-API runtime output artifacts (release)")
        set(NAPI_JAVASCRIPT_RUNTIME_OUTPUT_ARTIFACTS ${V8JSI_DLL_PATH} CACHE STRING "N-API runtime output artifacts")
        target_compile_definitions(jsi INTERFACE V8JSI_ENABLE_INSPECTOR)

        set(SOURCES ${SOURCES} "source/jsi.cpp")
    else()
        message(FATAL_ERROR "jsi target is required")
    endif()
endif()

add_library(napi ${SOURCES})

if (DEFINED V8JSI_PACKAGE_PATH)
    target_include_directories(napi 
        PUBLIC "${V8JSI_PACKAGE_PATH}/build/native/include"
        PUBLIC "${V8JSI_PACKAGE_PATH}/build/native/jsi")
endif()

target_include_directories(napi 
    PUBLIC "include")

target_link_to_dependencies(napi
    PUBLIC jsi)

if(WIN32)
    target_compile_definitions(napi PUBLIC _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING)
endif()

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

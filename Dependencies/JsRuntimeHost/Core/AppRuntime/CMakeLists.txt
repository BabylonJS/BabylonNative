if(APPLE)
    set(IMPL_EXT "mm")
else()
    set(IMPL_EXT "cpp")
endif()

set(SOURCES
    "Include/Babylon/Dispatchable.h"
    "Include/Babylon/AppRuntime.h"
    "Source/AppRuntime.cpp"
    "Source/AppRuntime_${NAPI_JAVASCRIPT_ENGINE}.cpp"
    "Source/AppRuntime_${JSRUNTIMEHOST_PLATFORM}.${IMPL_EXT}"
    "Source/WorkQueue.cpp"
    "Source/WorkQueue.h")

add_library(AppRuntime ${SOURCES})
warnings_as_errors(AppRuntime)

target_include_directories(AppRuntime
    PRIVATE "Include/Babylon"
    INTERFACE "Include")

target_link_libraries(AppRuntime
    PRIVATE arcana
    PUBLIC JsRuntime)

if(NAPI_JAVASCRIPT_ENGINE STREQUAL "V8" AND JSRUNTIMEHOST_CORE_APPRUNTIME_V8_INSPECTOR)
    add_subdirectory(V8Inspector)

    target_link_libraries(AppRuntime
        PRIVATE v8inspector)

    target_compile_definitions(AppRuntime
        PRIVATE ENABLE_V8_INSPECTOR)

    target_link_libraries(AppRuntime
        PRIVATE v8inspector)

    set_property(TARGET v8inspector PROPERTY FOLDER Dependencies)
endif()

set_property(TARGET AppRuntime PROPERTY FOLDER Core)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

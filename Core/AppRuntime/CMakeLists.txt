if(NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")

    set(SOURCES
        "Include/Babylon/AppRuntime.h"
        "Source/AppRuntime.cpp"
        "Source/AppRuntime${NAPI_JAVASCRIPT_ENGINE}.cpp"
        "Source/WorkQueue.cpp"
        "Source/WorkQueue.h")

    set(ENGINE_APP_RUNTIME_HEADER "Source/${NAPI_JAVASCRIPT_ENGINE}/Babylon/AppRuntime${NAPI_JAVASCRIPT_ENGINE}.h")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ENGINE_APP_RUNTIME_HEADER}")
        set(SOURCES ${SOURCES} ${ENGINE_APP_RUNTIME_HEADER})
    endif()

    if(APPLE)
        set(SOURCES ${SOURCES} "Source/AppRuntime${BABYLON_NATIVE_PLATFORM}.mm")
    else()
        set(SOURCES ${SOURCES} "Source/AppRuntime${BABYLON_NATIVE_PLATFORM}.cpp")
    endif()

    add_library(AppRuntime ${SOURCES})
    warnings_as_errors(AppRuntime)

    target_include_directories(AppRuntime
        PRIVATE "Include/Babylon"
        INTERFACE "Include")

    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${ENGINE_APP_RUNTIME_HEADER}")
        target_include_directories(AppRuntime 
            PRIVATE "Source/${NAPI_JAVASCRIPT_ENGINE}/Babylon"
            INTERFACE "Source/${NAPI_JAVASCRIPT_ENGINE}")
    endif()

    if(UNIX AND NOT APPLE AND NOT ANDROID)
        target_include_directories(AppRuntime INTERFACE "/usr/include/webkitgtk-4.0/")
    endif()

    target_link_to_dependencies(AppRuntime
        PRIVATE arcana
        PUBLIC JsRuntime)

    target_compile_definitions(AppRuntime
        PRIVATE NOMINMAX)

    set_property(TARGET AppRuntime PROPERTY FOLDER Core)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

endif()
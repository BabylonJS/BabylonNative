if(NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")

    set(SOURCES
        "Include/Babylon/AppRuntime.h"
        "Source/AppRuntime.cpp"
        "Source/AppRuntime${NAPI_JAVASCRIPT_ENGINE}.cpp"
        "Source/WorkQueue.cpp"
        "Source/WorkQueue.h")

    if (NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
        set(SOURCES ${SOURCES}
            "Source/v8/V8Inc.h"
            "Source/v8/V8InspectorAgent.h"
            "Source/v8/V8InspectorAgent.cpp"
            "Source/v8/V8InspectorSocket.h"
            "Source/v8/V8InspectorSocket.cpp"
            "Source/v8/V8InspectorSocketServer.h"
            "Source/v8/V8InspectorSocketServer.cpp"
            "Source/v8/V8InspectorTCP.h"
            "Source/v8/V8InspectorTCP.cpp"
            "Source/v8/V8InspectorUtils.h"
            "Source/v8/V8InspectorUtils.cpp")
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

    if(UNIX AND NOT APPLE AND NOT ANDROID)
        target_include_directories(AppRuntime INTERFACE "/usr/include/webkitgtk-4.0/")
    endif()

    if (NAPI_JAVASCRIPT_ENGINE STREQUAL "V8")
        target_link_to_dependencies(AppRuntime PRIVATE asio)
        target_compile_definitions(AppRuntime PRIVATE ASIO_STANDALONE)
        target_link_to_dependencies(AppRuntime PRIVATE llhttp)
    endif()

    target_link_to_dependencies(AppRuntime
        PRIVATE arcana
        PUBLIC JsRuntime)

    target_compile_definitions(AppRuntime
        PRIVATE NOMINMAX)

    set_property(TARGET AppRuntime PROPERTY FOLDER Core)
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})

endif()
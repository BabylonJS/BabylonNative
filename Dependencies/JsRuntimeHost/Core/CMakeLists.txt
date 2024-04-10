if(NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    add_subdirectory(Node-API-JSI)
else()
    add_subdirectory(Node-API)
endif()

add_subdirectory(Foundation)
add_subdirectory(JsRuntime)

if(JSRUNTIMEHOST_CORE_APPRUNTIME)
    add_subdirectory(AppRuntime)
endif()

if(JSRUNTIMEHOST_CORE_SCRIPTLOADER)
    add_subdirectory(ScriptLoader)
endif()

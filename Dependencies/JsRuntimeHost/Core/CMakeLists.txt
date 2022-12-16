add_subdirectory(Node-API)
add_subdirectory(JsRuntime)

if(JSRUNTIMEHOST_CORE_APPRUNTIME)
    add_subdirectory(AppRuntime)
endif()

if(JSRUNTIMEHOST_CORE_SCRIPTLOADER)
    add_subdirectory(ScriptLoader)
endif()

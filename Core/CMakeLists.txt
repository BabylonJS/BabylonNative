add_subdirectory(JsRuntime)
add_subdirectory(Graphics)

if(BABYLON_NATIVE_CORE_APPRUNTIME)
    add_subdirectory(AppRuntime)
endif()

if(BABYLON_NATIVE_CORE_SCRIPTLOADER)
    add_subdirectory(ScriptLoader)
endif()

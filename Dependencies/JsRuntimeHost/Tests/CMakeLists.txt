add_subdirectory(UnitTests)
set(NODE_API_ANDROID_ENGINES V8 JavaScriptCore)
if(NOT IOS AND NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI"
        AND (NOT ANDROID OR NAPI_JAVASCRIPT_ENGINE IN_LIST NODE_API_ANDROID_ENGINES))
    add_subdirectory(NodeApi)
endif()
npm(install --silent)

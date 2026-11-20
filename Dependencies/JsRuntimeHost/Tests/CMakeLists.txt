add_subdirectory(UnitTests)

# The Node-API conformance harness (node_lite + the js-native-api addons) is written against the C
# napi_* API. The JSI backend (Core/Node-API-JSI) implements only the C++ `Napi::` wrapper over jsi and
# exports no C napi_* symbols, and its napi.h carries its own copy of the napi types -- which collides
# with the shared <napi/js_native_api_types.h> the harness includes (C2365 redefinition). Skip the
# harness for JSI rather than trying to reconcile the two type sources.
if(NOT NAPI_JAVASCRIPT_ENGINE STREQUAL "JSI")
    add_subdirectory(NodeApi)
endif()

npm(install --silent)

#ifndef NODE_API_TESTS_JS_NATIVE_API_H_
#define NODE_API_TESTS_JS_NATIVE_API_H_

// Test-only adapter between the vendored Node.js test sources and the shared
// JsRuntimeHost headers. Test modules resolve <js_native_api.h> here first.
#include <napi/js_native_api.h>

// Upstream Node.js headers define these source-compatibility aliases at every
// Node-API version. Without NAPI_EXPERIMENTAL they are the ordinary types.
typedef napi_env node_api_nogc_env;
typedef node_api_nogc_env node_api_basic_env;
typedef napi_finalize node_api_nogc_finalize;
typedef node_api_nogc_finalize node_api_basic_finalize;

// The shared header replaces the standard three-argument napi_run_script with
// a four-argument variant that adds source_url, which Chakra and V8
// dereference unconditionally.
#define napi_run_script(env, script, result) \
  napi_run_script((env), (script), "", (result))

#endif  // NODE_API_TESTS_JS_NATIVE_API_H_

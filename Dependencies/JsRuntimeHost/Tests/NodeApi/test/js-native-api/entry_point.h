#ifndef JS_NATIVE_API_ENTRY_POINT_H_
#define JS_NATIVE_API_ENTRY_POINT_H_

#include <node_api.h>

#if defined(JSR_NODE_API_STATIC_LINK)
// Static-link mode (the Android in-process conformance runner): every addon is linked into one host
// binary, so Init must have internal linkage to avoid one-definition clashes across addons. The
// registrar/version functions are uniquely suffixed per-module by the build (NODE_API_MODULE_*_FUNCTION
// in node_api.h). A later non-static definition of Init in the addon inherits this internal linkage.
static napi_value Init(napi_env env, napi_value exports);
#else
EXTERN_C_START
napi_value Init(napi_env env, napi_value exports);
EXTERN_C_END
#endif

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

#if defined(JSR_NODE_API_STATIC_LINK)
// Self-register this addon's uniquely-named entry points with the host at load time, keyed by module
// name, so the in-process loader (node_lite_android LoadFunction) can find them by name -- the static
// equivalent of dlopen+dlsym. jsr_register_static_addon is implemented by the host.
EXTERN_C_START
void jsr_register_static_addon(
    const char* name,
    int32_t (*get_api_version)(void),
    napi_value (*register_module)(napi_env, napi_value));
EXTERN_C_END

static void __attribute__((constructor)) jsr_register_static_addon_ctor(void) {
  jsr_register_static_addon(NODE_GYP_MODULE_NAME,
                            NODE_API_MODULE_GET_API_VERSION_FUNCTION,
                            NODE_API_MODULE_REGISTER_FUNCTION);
}
#endif

#endif  // JS_NATIVE_API_ENTRY_POINT_H_

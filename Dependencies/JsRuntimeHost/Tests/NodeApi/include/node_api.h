#ifndef NODE_API_H_
#define NODE_API_H_

#include <napi/js_native_api.h>
#include "node_api_types.h"

#ifdef __cplusplus
#define NODE_API_EXTERN_C_START extern "C" {
#define NODE_API_EXTERN_C_END }
#else
#define NODE_API_EXTERN_C_START
#define NODE_API_EXTERN_C_END
#endif

#ifdef _WIN32
#define NAPI_MODULE_EXPORT __declspec(dllexport)
#else
#define NAPI_MODULE_EXPORT __attribute__((visibility("default")))
#endif

#ifndef NAPI_MODULE_VERSION
#define NAPI_MODULE_VERSION 1
#endif

typedef napi_value(NAPI_CDECL* napi_addon_register_func)(napi_env env,
                                                         napi_value exports);

typedef struct napi_module_s {
    int nm_version;
    unsigned int nm_flags;
    const char* nm_filename;
    napi_addon_register_func nm_register_func;
    const char* nm_modname;
    void* nm_priv;
    void* reserved[4];
} napi_module;

#define NODE_API_MODULE_GET_API_VERSION_FUNCTION node_api_module_get_api_version_v1
#define NODE_API_MODULE_REGISTER_FUNCTION napi_register_module_v1

#define NAPI_MODULE_INIT()                                                     \
    NODE_API_EXTERN_C_START                                                    \
    NAPI_MODULE_EXPORT int32_t NODE_API_MODULE_GET_API_VERSION_FUNCTION(void) {\
        return NAPI_VERSION;                                                   \
    }                                                                          \
    NAPI_MODULE_EXPORT napi_value NODE_API_MODULE_REGISTER_FUNCTION(           \
        napi_env env, napi_value exports);                                     \
    NODE_API_EXTERN_C_END                                                      \
    static napi_value napi_module_init_impl(napi_env env, napi_value exports); \
    NODE_API_EXTERN_C_START                                                    \
    NAPI_MODULE_EXPORT napi_value NODE_API_MODULE_REGISTER_FUNCTION(           \
        napi_env env, napi_value exports) {                                    \
        return napi_module_init_impl(env, exports);                            \
    }                                                                          \
    NODE_API_EXTERN_C_END                                                      \
    static napi_value napi_module_init_impl(napi_env env, napi_value exports)

#define NAPI_MODULE(modname, regfunc)                                          \
    NAPI_MODULE_INIT() {                                                       \
        (void)(modname);                                                       \
        return regfunc(env, exports);                                          \
    }

#define NAPI_MODULE_X(modname, regfunc, priv, flags)                           \
    NAPI_MODULE(modname, regfunc)

#endif  // NODE_API_H_

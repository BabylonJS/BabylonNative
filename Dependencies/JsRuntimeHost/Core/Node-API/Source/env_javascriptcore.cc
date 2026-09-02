#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "js_native_api_javascriptcore.h"

namespace Napi
{
    Napi::Env Attach(JSGlobalContextRef context)
    {
        napi_env env_ptr{new napi_env__{context}};
        return {env_ptr};
    }

    void Detach(Napi::Env env)
    {
        napi_env env_ptr{env};
        delete env_ptr;
    }

    JSGlobalContextRef GetContext(Napi::Env env)
    {
        napi_env env_ptr{env};
        return env_ptr->context;
    }
}

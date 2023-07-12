#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "js_native_api_v8.h"
#include <libplatform/libplatform.h>

namespace Napi
{
    template<>
    Env Attach<v8::Local<v8::Context>>(v8::Local<v8::Context> isolate)
    {
        return {new napi_env__(isolate)};
    }

    template<>
    v8::Local<v8::Context> GetContext(Env env)
    {
        return ((napi_env)env)->context();
    }

    void Detach(Env env)
    {
        napi_env env_ptr{env};
        delete env_ptr;
    }
}

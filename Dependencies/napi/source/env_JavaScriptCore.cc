#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "JavaScriptCore/JavaScript.h"
#include "js_native_api_JavaScriptCore.h"

namespace Napi
{
    template<>
    Napi::Env Attach<JSContextRef>(JSContextRef context)
    {
        auto envPtr = new napi_env__();
        envPtr->context = context;
        return{ envPtr };
    }

    void Detach(Napi::Env env)
    {
        delete env.operator napi_env();
    }

    template<> JSContextRef GetContext(Napi::Env env)
    {
        napi_env napienv = env;
        return napienv->context;
    }
}

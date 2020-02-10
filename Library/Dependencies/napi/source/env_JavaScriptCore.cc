#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "JavaScriptCore/JavaScriptCore.h"
#include "js_native_api_JavaScriptCore.h"

namespace Napi
{
    template<>
    void Attach<JSGlobalContextRef>(JSGlobalContextRef globalContext)
    {
        return{ new napi_env__(globalContext) };
    }

    void Detach(Napi::Env env)
    {
        delete env.operator napi_env();
    }
}

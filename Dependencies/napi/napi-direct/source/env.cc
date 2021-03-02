#include <napi/env.h>

namespace Napi
{
    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        napi_value result;

        NAPI_THROW_IF_FAILED(env, napi_run_script(env, Napi::String::New(env, source), sourceUrl, &result));
        return{ env, result };
    }
}

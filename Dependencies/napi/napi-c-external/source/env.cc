#include <napi/env.h>

namespace Napi
{
    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        napi_value script;
        NAPI_THROW_IF_FAILED(env, napi_create_string_utf8(env, source, NAPI_AUTO_LENGTH, &script));

        napi_value result;
        NAPI_THROW_IF_FAILED(env, napi_run_script(env, script, &result));
        return{ env, result };
    }
}

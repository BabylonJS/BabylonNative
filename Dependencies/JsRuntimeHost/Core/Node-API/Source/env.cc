#include <napi/env.h>

namespace Napi
{
    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        return env.RunScript(source, sourceUrl);
    }
}

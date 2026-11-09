#include <napi/env.h>

namespace Napi
{
    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        return env.RunScript(source, sourceUrl);
    }

    Napi::Value Eval(Napi::Env env, std::string_view source, const char* sourceUrl)
    {
        return env.RunScript(Napi::String::New(env, source.data(), source.size()), sourceUrl);
    }
}

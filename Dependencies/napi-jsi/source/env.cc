#include <napi/env.h>

namespace Napi
{
    template<>
    Env Attach<>(facebook::jsi::Runtime* rt)
    {
        return {rt};
    }

    void Detach(Env env)
    {
    }

    Napi::Value Eval(Napi::Env env, const char* string, const char* sourceUrl)
    {
        napi_env env_ptr{env};
        return {env_ptr, {env_ptr, env_ptr->evaluateJavaScript(std::make_shared<facebook::jsi::StringBuffer>(string), sourceUrl)}};
    }
}

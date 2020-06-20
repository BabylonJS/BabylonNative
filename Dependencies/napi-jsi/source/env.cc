#include <napi/env.h>

namespace Napi
{
    template<>
    Env Attach<>(facebook::jsi::Runtime& rt)
    {
      napi_env__* env_ptr{new napi_env__{rt}};
      return {env_ptr};
    }

    void Detach(Env env)
    {
    }

    Napi::Value Eval(Napi::Env env, const char* string, const char* sourceUrl)
    {
      napi_env__* env_ptr{env};
      return {env_ptr, env_ptr->rt.evaluateJavaScript(std::make_shared<facebook::jsi::StringBuffer>(string), sourceUrl)};
    }
}

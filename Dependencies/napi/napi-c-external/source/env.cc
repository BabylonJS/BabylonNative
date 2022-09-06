#include <napi/env.h>
#include <napi/js_native_ext_api.h>

namespace Napi
{
    struct EnvScope 
    {
    EnvScope(napi_env env) noexcept : m_env{env}
    {
        napi_open_handle_scope(env, &handle_scope_);
    }
    ~EnvScope() noexcept{
        napi_close_handle_scope(m_env, handle_scope_);
    }
    EnvScope(const EnvScope &) = delete;
    EnvScope &operator=(const EnvScope &) = delete;

    private:
    napi_env m_env{};
    napi_handle_scope handle_scope_{};
    };
      
    Napi::Value Eval(Napi::Env env, const char* source, const char* sourceUrl)
    {
        EnvScope envScope(env);

        napi_value script;
        NAPI_THROW_IF_FAILED(env, napi_create_string_utf8(env, source, NAPI_AUTO_LENGTH, &script));

        napi_value result{};
        NAPI_THROW_IF_FAILED(env, napi_ext_run_script(env, script, sourceUrl, &result));

        return{ env, result };
    }
}

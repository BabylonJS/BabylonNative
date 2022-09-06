#include "AppRuntime.h"
#include <exception>
#include <sstream>
#include <functional>
#include <napi/js_native_api.h>
#include <napi/js_native_ext_api.h>

namespace Babylon
{
    void AppRuntime::RunEnvironmentTier(const char*)
    {
        napi_env _env{};
        napi_ext_env_settings settings{};
        settings.this_size = sizeof(settings);
        settings.inspector_port = 5643;
        settings.flags.enable_gc_api = true;
        settings.flags.enable_inspector = 1;
        settings.flags.wait_for_debugger = false;

        (napi_ext_create_env(&settings, &_env));
        Napi::Env env = Napi::Env(_env);

        napi_ext_env_scope scope;
        NAPI_THROW_IF_FAILED(env, napi_ext_open_env_scope(env, &scope));

        Run(env);
        NAPI_THROW_IF_FAILED(env, napi_ext_close_env_scope(env, scope));
    }
}
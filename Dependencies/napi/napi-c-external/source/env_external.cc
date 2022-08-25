#include "js_native_api_external.h"
#include <napi/js_native_api.h>
#include <napi/js_native_ext_api.h>
#include <array>
#include <cassert>
#include <cmath>
#include <vector>
#include <string>
#include <stdexcept>
#include "napi/env.h"


namespace Napi
{
    template<>
    Env Attach<>()
    {
        napi_env env;
        napi_ext_env_settings settings{};
        settings.this_size = sizeof(settings);
        settings.flags.enable_gc_api = true;
        settings.flags.enable_inspector = 1;
        settings.flags.wait_for_debugger = true;
        settings.foreground_scheduler = nullptr;

        napi_ext_create_env(&settings, &env);
        
        // Associate environment to holder.
        //napi_set_instance_data(env, this, nullptr /*finalize_cb*/, nullptr /*finalize_hint*/);
        return {env};
    }

    void Detach(Env env)
    {
        //napi_env env_ptr{env};
        //delete env_ptr;
    }
}

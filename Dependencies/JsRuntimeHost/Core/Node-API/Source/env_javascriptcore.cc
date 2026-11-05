#include <napi/env.h>
#include <napi/js_native_api_types.h>
#include "js_native_api_javascriptcore.h"
#include <memory>
#include <stdexcept>

namespace Napi
{
    Napi::Env Attach(JSGlobalContextRef context)
    {
        auto env_ptr{std::make_unique<napi_env__>(context)};
        if (napi_shared::CapturePropertyNameIntrinsics(env_ptr.get(), env_ptr->property_name_intrinsics) != napi_ok)
        {
            throw std::runtime_error{"Napi::Attach: failed to capture property-name intrinsics"};
        }
        return {env_ptr.release()};
    }

    void Detach(Napi::Env env)
    {
        napi_env env_ptr{env};
        if (napi_shared::ReleasePropertyNameIntrinsics(env_ptr, env_ptr->property_name_intrinsics) != napi_ok)
        {
            throw std::runtime_error{"Napi::Detach: failed to release property-name intrinsics"};
        }
        delete env_ptr;
    }

    JSGlobalContextRef GetContext(Napi::Env env)
    {
        napi_env env_ptr{env};
        return env_ptr->context;
    }
}

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
#include <iostream>


namespace Napi
{

    template<>
    Env Attach<>(napi_env& rt)
    {
        napi_env__* env_ptr{new napi_env__{rt}};
        return {env_ptr};
    }

    void Detach(Env env)
    {
        napi_env__* env_ptr{env};
        delete env_ptr;
    }
}

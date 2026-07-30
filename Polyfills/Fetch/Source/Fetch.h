#pragma once

#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    namespace Fetch
    {
        void Initialize(Napi::Env env);
    }
}

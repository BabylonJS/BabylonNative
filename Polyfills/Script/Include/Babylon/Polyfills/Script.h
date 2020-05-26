#pragma once

#include <napi/env.h>

namespace Babylon::Polyfills::Script
{
    using DispatchFunctionT = std::function<void(std::function<void(Napi::Env)>)>;

    void Initialize(Napi::Env env, DispatchFunctionT dispathFunction);
}

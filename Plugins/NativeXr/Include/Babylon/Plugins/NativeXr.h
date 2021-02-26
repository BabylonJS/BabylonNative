#pragma once

#include <napi/env.h>

namespace Babylon::Plugins::NativeXr
{
    void Initialize(Napi::Env env);
    void SetSessionStateChangedCallback(Napi::Env env, std::function<void(bool)> callback);
}

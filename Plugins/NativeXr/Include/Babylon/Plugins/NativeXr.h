#pragma once

#include <napi/env.h>

namespace Babylon::Plugins::NativeXr
{
    struct Configuration
    {
        std::function<void(bool)> SessionStateChangedCallback{};
    };

    void Initialize(Napi::Env env, Configuration config = {});
}

#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeXr
    {
    public:
        struct Configuration
        {
            std::function<void(bool)> SessionStateChangedCallback{};
            std::function<void*()> WindowProvider{};
        };

        static void Initialize(Napi::Env env, Configuration config);
    };
}

//namespace Babylon::Plugins::NativeXr
//{
//    struct Configuration
//    {
//        std::function<void(bool)> SessionStateChangedCallback{};
//        std::function<void*()> WindowProvider{};
//    };
//
//    void Initialize(Napi::Env env, Configuration config = {});
//}

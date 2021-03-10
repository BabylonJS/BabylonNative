#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeXr final
    {
    public:
        NativeXr(const NativeXr& other) = delete;
        NativeXr(NativeXr&&) = default;

        struct Configuration
        {
            std::function<void(bool)> SessionStateChangedCallback{};
            std::function<void*()> WindowProvider{};
        };

        static NativeXr Initialize(Napi::Env env, Configuration config);
        ~NativeXr();

    private:
        class Impl;
        std::unique_ptr<Impl> m_impl{};
        NativeXr(std::unique_ptr<Impl> impl);
    };
}

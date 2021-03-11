#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeXr final
    {
    public:
        class Impl;

        NativeXr(std::shared_ptr<Impl> impl);
        NativeXr(const NativeXr& other) = default;
        NativeXr(NativeXr&&) = default;
        ~NativeXr();

        struct Configuration
        {
            std::function<void(bool)> SessionStateChangedCallback{};
        };

        static NativeXr Initialize(Napi::Env env, Configuration config);

        void UpdateWindow(void* windowPtr);

    private:
        std::shared_ptr<Impl> m_impl{};
    };
}

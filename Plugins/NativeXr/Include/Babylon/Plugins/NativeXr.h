#pragma once

#include <napi/env.h>

namespace Babylon::Plugins
{
    class NativeXr final
    {
    public:
        class Impl;

        NativeXr(const NativeXr& other) = default;
        NativeXr(NativeXr&&) = default;
        ~NativeXr();

        static NativeXr Initialize(Napi::Env env);

        void UpdateWindow(void* windowPtr);
        void SetSessionStateChangedCallback(std::function<void(bool)> callback);

    private:
        NativeXr(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
}

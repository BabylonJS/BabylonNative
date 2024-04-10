#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Plugins
{
    class NativeXr final
    {
    public:
        class Impl;

        NativeXr(const NativeXr& other) = default;
        NativeXr& operator=(const NativeXr& other) = default;

        NativeXr(NativeXr&&) noexcept = default;
        NativeXr& operator=(NativeXr&&) noexcept = default;

        ~NativeXr();

        static NativeXr BABYLON_API Initialize(Napi::Env env);

        void UpdateWindow(void* windowPtr);
        void SetSessionStateChangedCallback(std::function<void(bool)> callback);

    private:
        NativeXr(std::shared_ptr<Impl> impl);
        std::shared_ptr<Impl> m_impl{};
    };
}

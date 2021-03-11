#pragma once

#include <napi/env.h>
#include <arcana/containers/ticketed_collection.h>

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

        static NativeXr Initialize(Napi::Env env);

        void UpdateWindow(void* windowPtr);

        using SessionStateChangedCallback = std::function<void(bool)>;
        using SessionStateChangedCallbackTicket = arcana::ticketed_collection<SessionStateChangedCallback>::ticket;
        SessionStateChangedCallbackTicket AddSessionStateChangedCallback(SessionStateChangedCallback callback);

    private:
        std::shared_ptr<Impl> m_impl{};
    };
}

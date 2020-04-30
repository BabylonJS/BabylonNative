#pragma once

#include <arcana/containers/weak_table.h>

#include <napi/env.h>

#include <functional>

namespace Babylon
{
    class AppLifeCycle
    {
    public:
        using CallbackT = std::function<void()>;
        using TicketT = arcana::weak_table<CallbackT>::ticket;

        static AppLifeCycle& CreateForJavaScript(Napi::Env);
        static AppLifeCycle& GetFromJavaScript(Napi::Env);

        AppLifeCycle(const AppLifeCycle&) = delete;
        AppLifeCycle(AppLifeCycle&&) = delete;

        TicketT AddOnSuspendCallback(CallbackT);
        TicketT AddOnResumeCallback(CallbackT);

        void Suspend();
        void Resume();

    private:
        AppLifeCycle(Napi::Env);

        arcana::weak_table<CallbackT> m_suspendCallbacks{};
        arcana::weak_table<CallbackT> m_resumeCallbacks{};
    };
}
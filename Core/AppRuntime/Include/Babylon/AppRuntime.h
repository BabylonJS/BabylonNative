#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>
#include <string>

namespace Babylon
{
    class WorkQueue;

    class AppRuntime final : public JsRuntime
    {
    public:
        AppRuntime(const char* rootUrl = nullptr);
        ~AppRuntime();

        void Suspend();
        void Resume();

        void Dispatch(std::function<void(Napi::Env)> callback);

        const std::string RootUrl;

    private:
        void RunPlatformTier();
        void RunEnvironmentTier();
        void Run(Napi::Env);
        std::unique_ptr<WorkQueue> m_workQueue{};
    };
}

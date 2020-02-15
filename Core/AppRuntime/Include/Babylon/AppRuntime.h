#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>
#include <string>

namespace Babylon
{
    class TaskChain;

    class AppRuntime final : public JsRuntime
    {
    public:
        AppRuntime(const char* rootUrl = nullptr);

        const std::string& GetRootUrl() const;

        void Suspend();
        void Resume();

        void Dispatch(std::function<void(Napi::Env)> callback);

    private:
        void CreateEnvironmentAndRun();
        void Run(Napi::Env);

        const std::string m_rootUrl{};
        std::unique_ptr<TaskChain> m_taskChain{};
    };
}

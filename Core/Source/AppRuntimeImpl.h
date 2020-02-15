#pragma once

#include "AppRuntime.h"

#include "Common.h"
#include "TaskChain.h"

#include <arcana/threading/cancellation.h>
#include <arcana/threading/task.h>

#include <mutex>
#include <thread>

namespace Babylon
{
    class AppRuntime::Impl final
    {
    public:
        Impl(AppRuntime& runtime, const char* rootUrl);
        Impl(const Impl&) = delete;
        
    private:
        friend class AppRuntime;

        void CreateEnvironmentAndRun();
        void Run(Napi::Env);

        AppRuntime& Runtime;
        const std::string RootUrl{};
        std::unique_ptr<TaskChain> TaskChain{};
    };
}
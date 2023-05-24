#pragma once

#include <napi/env.h>

#include "Dispatchable.h"

#include <memory>
#include <functional>
#include <exception>

namespace Babylon
{
    class AppRuntimeImpl;

    class AppRuntime final
    {
    public:
        AppRuntime();
        AppRuntime(std::function<void(const std::exception&)> unhandledExceptionHandler);
        ~AppRuntime();

        // Move semantics
        AppRuntime(AppRuntime&&) noexcept;
        AppRuntime& operator=(AppRuntime&&) noexcept;

        void Suspend();
        void Resume();

        void Dispatch(Dispatchable<void(Napi::Env)> callback);

    private:
        std::unique_ptr<AppRuntimeImpl> m_impl;
    };
}

#pragma once

#include "AppRuntime.h"
#include <optional>
#include <mutex>
#include <arcana/threading/dispatcher.h>

namespace Babylon
{
    class AppRuntimeImpl
    {
    public:
        AppRuntimeImpl(std::function<void(const std::exception&)> unhandledExceptionHandler = DefaultUnhandledExceptionHandler);
        ~AppRuntimeImpl();

        void Suspend();
        void Resume();

        void Dispatch(Dispatchable<void(Napi::Env)> func);

    private:
        static void DefaultUnhandledExceptionHandler(const std::exception& error);

        template<typename CallableT>
        void Append(CallableT callable)
        {
            // Manual dispatcher queueing requires a copyable CallableT, we use a shared pointer trick to make a
            // copyable callable if necessary.
            if constexpr (std::is_copy_constructible<CallableT>::value)
            {
                m_dispatcher.queue([this, callable = std::move(callable)]() {
                    callable(m_env.value());
                });
            }
            else
            {
                m_dispatcher.queue([this, callablePtr = std::make_shared<CallableT>(std::move(callable))]() {
                    (*callablePtr)(m_env.value());
                });
            }
        }

        // These three methods are the mechanism by which platform- and JavaScript-specific
        // code can be "injected" into the execution of the JavaScript thread. These three
        // functions are implemented in separate files, thus allowing implementations to be
        // mixed and matched by the build system based on the platform and JavaScript engine
        // being targeted, without resorting to virtuality. An important nuance of these
        // functions is that they are all intended to call each other: RunPlatformTier MUST
        // call RunEnvironmentTier, which MUST create the initial Napi::Env and pass it to
        // Run. This arrangement allows not only for an arbitrary assemblage of platforms,
        // but it also allows us to respect the requirement by certain platforms (notably V8)
        // that certain program state be allocated and stored only on the stack.
        void RunPlatformTier();
        void RunEnvironmentTier(const char* executablePath = ".");
        void Run(Napi::Env);

        // This method is called from Dispatch to allow platform-specific code to add
        // extra logic around the invocation of a dispatched callback.
        void Execute(Dispatchable<void()> callback);

        std::function<void(const std::exception&)> m_unhandledExceptionHandler{};
        std::optional<Napi::Env> m_env{};
        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};
        arcana::cancellation_source m_cancellationSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};
        std::thread m_thread{};
    };
}

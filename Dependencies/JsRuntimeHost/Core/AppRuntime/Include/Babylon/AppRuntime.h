#pragma once

#include "Dispatchable.h"

#include <Babylon/JsRuntime.h>

#include <napi/utilities.h>

#include <arcana/threading/cancellation.h>
#include <arcana/threading/dispatcher.h>

#include <memory>
#include <functional>
#include <exception>
#include <optional>
#include <mutex>
#include <thread>
#include <type_traits>

namespace Babylon
{
    class AppRuntime final
    {
    public:
        class Options
        {
        public:
            // Optional handler for unhandled exceptions.
            std::function<void(const Napi::Error&)> UnhandledExceptionHandler{DefaultUnhandledExceptionHandler};

            // Defines whether to enable the debugger. Only implemented for V8 and Chakra.
            bool EnableDebugger{false};

            // Waits for the debugger to be attached before the execution of any script. Only implemented for V8.
            bool WaitForDebugger{false};
        };

        AppRuntime();
        AppRuntime(Options options);
        ~AppRuntime();

        void Suspend();
        void Resume();

        void Dispatch(Dispatchable<void(Napi::Env)> callback);

        // Default unhandled exception handler that outputs the error message to the program output.
        static void BABYLON_API DefaultUnhandledExceptionHandler(const Napi::Error& error);

    private:
        template<typename CallableT>
        void Append(CallableT callable)
        {
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

        Options m_options;
        std::optional<Napi::Env> m_env{};
        std::optional<std::scoped_lock<std::mutex>> m_suspensionLock{};
        arcana::cancellation_source m_cancelSource{};
        arcana::manual_dispatcher<128> m_dispatcher{};
        std::thread m_thread;
    };
}

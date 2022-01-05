#pragma once

#include <Babylon/JsRuntime.h>

#include <memory>
#include <functional>
#include <exception>

namespace Babylon
{
    template<typename...>
    class dispatchable_impl_base;

    template<typename ReturnT, typename... ArgsT>
    class dispatchable_impl_base<ReturnT(ArgsT...)>
    {
    public:
        virtual ~dispatchable_impl_base()
        {
        }

        virtual ReturnT operator()(ArgsT... args) = 0;
    };

    template<typename...>
    class dispatchable_impl;

    template<typename CallableT, typename ReturnT, typename... ArgsT>
    class dispatchable_impl<CallableT, ReturnT(ArgsT...)> : public dispatchable_impl_base<ReturnT(ArgsT...)>
    {
    public:
        dispatchable_impl(dispatchable_impl<CallableT, ReturnT(ArgsT...)>&& other) = default;
        
        dispatchable_impl(CallableT&& callable)
            : m_callable{std::forward<CallableT>(callable)}
        {
        }

        ReturnT operator()(ArgsT... args) override
        {
            m_callable(args...);
        }

    private:
        CallableT m_callable;
    };

    template<typename SignatureT>
    class dispatchable
    {
    public:
        dispatchable() = default;
        dispatchable(const dispatchable&) = delete;
        dispatchable(dispatchable&&) = default;

        template<typename CallableT>
        dispatchable(CallableT&& callable)
            : m_impl{std::make_unique<dispatchable_impl<CallableT, SignatureT>>(std::forward<CallableT>(callable))}
        {
        }

        template<typename... ArgsT>
        void operator()(ArgsT... args)
        {
            (*m_impl)(args...);
        }

    private:
        std::unique_ptr<dispatchable_impl_base<SignatureT>> m_impl{};
    };

    class WorkQueue;
    class AppRuntime final
    {
    public:
        AppRuntime();
        AppRuntime(std::function<void(std::exception_ptr)> unhandledExceptionHandler);
        ~AppRuntime();

        void Suspend();
        void Resume();

        void Dispatch(dispatchable<void(Napi::Env)> callback);

    private:
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
        void Execute(dispatchable<void()> callback);

        static void DefaultUnhandledExceptionHandler(std::exception_ptr ptr);

        std::unique_ptr<WorkQueue> m_workQueue;
    };
}

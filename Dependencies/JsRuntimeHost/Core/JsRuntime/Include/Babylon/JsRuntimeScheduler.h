#pragma once

#include "JsRuntime.h"

#include <type_traits>
#include <utility>

namespace Babylon
{
    /**
     * Scheduler that invokes continuations via JsRuntime::Dispatch.
     * Intended to be consumed by arcana.cpp tasks.
     * Copies can outlive the runtime; dispatch after shutdown is discarded.
     */
    class JsRuntimeScheduler
    {
    public:
        explicit JsRuntimeScheduler(JsRuntime& runtime)
            : m_runtimeState{runtime.m_state}
        {
        }

        template<typename CallableT>
        void operator()(CallableT&& callable) const
        {
            JsRuntime::Dispatch(m_runtimeState, [callable{std::forward<CallableT>(callable)}](Napi::Env env) mutable {
                // Preserve the original const, zero-argument invocation when available.
                if constexpr (std::is_invocable_v<const decltype(callable)&>)
                {
                    std::as_const(callable)();
                }
                else if constexpr (std::is_invocable_v<decltype(callable)&>)
                {
                    callable();
                }
                else
                {
                    callable(env);
                }
            });
        }

    private:
        std::shared_ptr<JsRuntime::InternalState> m_runtimeState;
    };
}

#pragma once

#include <Babylon/JsRuntime.h>
#include <napi/napi.h>

#include <chrono>
#include <cstdint>
#include <memory>

namespace Babylon::Polyfills::Internal
{
    class TimeoutDispatcher
    {
        using TimeoutId = int32_t;
        struct Timeout;

    public:
        TimeoutDispatcher(Napi::Env env, Babylon::JsRuntime& runtime);
        ~TimeoutDispatcher();

        TimeoutId Dispatch(std::shared_ptr<Napi::FunctionReference> function, std::chrono::milliseconds delay, bool repeat = false);
        void Clear(TimeoutId id);

    private:
        friend struct TimeoutDispatcherTestAccess;
        TimeoutDispatcher(Napi::Env env, Babylon::JsRuntime& runtime, TimeoutId lastTimeoutId);

        struct State;
        std::shared_ptr<State> m_state;
    };
}

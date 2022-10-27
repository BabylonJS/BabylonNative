#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <napi/napi.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <map>
#include <unordered_map>
#include <cstdint>
#include <thread>

namespace Babylon::Polyfills::Internal
{
    class TimeoutDispatcher
    {
        using TimeoutId = int32_t;
        struct Timeout;

    public:
        TimeoutDispatcher(Babylon::JsRuntime& runtime);
        ~TimeoutDispatcher();

        TimeoutId Dispatch(std::shared_ptr<Napi::FunctionReference> function, std::chrono::milliseconds delay);
        void Clear(TimeoutId id);

    private:
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>;

        TimeoutId NextTimeoutId();
        void ThreadFunction();
        void CallFunction(std::shared_ptr<Napi::FunctionReference> function);

        Babylon::JsRuntimeScheduler m_runtimeScheduler;

        mutable std::mutex m_mutex{};
        std::condition_variable m_condVariable{};
        TimeoutId m_lastTimeoutId{0};
        std::unordered_map<TimeoutId, std::unique_ptr<Timeout>> m_idMap{};
        std::multimap<TimePoint, Timeout*> m_timeMap{};
        arcana::cancellation_source m_cancellationSource{};
        std::thread m_thread{};
    };
}

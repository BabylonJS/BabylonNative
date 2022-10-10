#pragma once

#include <Babylon/JsRuntime.h>

#include <map>

namespace Babylon::Polyfills::Internal
{
    using TimeoutId = int32_t;

    struct Timeout;

    class TimeoutDispatcher
    {
    public:
        TimeoutDispatcher(Babylon::JsRuntime& runtime);
        ~TimeoutDispatcher();

        TimeoutId Dispatch(Napi::Function function, std::chrono::milliseconds delay);
        void Clear(TimeoutId id);

        TimeoutId NextTimeoutId();

    private:
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;

        void ThreadFunction();
        void CallFunction(std::shared_ptr<Napi::FunctionReference> function);

        Babylon::JsRuntime& m_runtime;
        std::thread m_thread;
        std::mutex m_mutex{};
        std::condition_variable m_condVariable{};
        TimeoutId m_lastTimeoutId = 0;
        std::unordered_map<TimeoutId, std::unique_ptr<Timeout>> m_idMap;
        std::multimap<TimePoint, Timeout*> m_timeMap;
        std::atomic<bool> m_shutdown{false};
    };
}

#pragma once

#include <Babylon/JsRuntime.h>

#include <map>

namespace Babylon::Polyfills::Internal
{
    struct Timeout;

    using TimeoutId = int32_t;

    class TimeoutDispatcher
    {
    public:
        TimeoutDispatcher(Babylon::JsRuntime& runtime);
        ~TimeoutDispatcher();

        TimeoutId Dispatch(Napi::Function func, std::chrono::milliseconds delay);
        void Clear(TimeoutId id);

    private:
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;

        void WaitThenCallProc();
        void CallFunction(std::shared_ptr<Napi::FunctionReference> func);
        TimeoutId NextTimeoutId();

        Babylon::JsRuntime& m_runtime;
        std::thread m_thread;
        std::mutex m_mutex{};
        std::condition_variable m_condVariable{};
        TimeoutId m_lastTimeoutId = 0;
        std::unordered_map<TimeoutId, std::unique_ptr<Timeout>> m_idMap;
        std::map<TimePoint, std::list<Timeout*>> m_timeMap;
        std::atomic<bool> m_shutdown{false};
    };
}

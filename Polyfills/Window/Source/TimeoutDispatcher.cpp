#include "TimeoutDispatcher.h"

#include <cassert>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::microseconds>;

        TimePoint Now()
        {
            return std::chrono::time_point_cast<std::chrono::microseconds, std::chrono::steady_clock>(std::chrono::steady_clock::now());
        }
    }

    struct TimeoutDispatcher::Timeout
    {
        TimeoutId id;

        // Make this non-shared when JsRuntime::Dispatch supports it.
        std::shared_ptr<Napi::FunctionReference> function;

        TimePoint time;

        Timeout(TimeoutId id, std::shared_ptr<Napi::FunctionReference> function, TimePoint time)
            : id{id}
            , function{std::move(function)}
            , time{time}
        {
        }

        Timeout(const Timeout&) = delete;
        Timeout(Timeout&&) = delete;
    };

    TimeoutDispatcher::TimeoutDispatcher(Babylon::JsRuntime& runtime)
        : m_runtime{runtime}
        , m_thread{std::thread{&TimeoutDispatcher::ThreadFunction, this}}
    {
    }

    TimeoutDispatcher::~TimeoutDispatcher()
    {
        {
            std::unique_lock<std::mutex> lk{m_mutex};
            m_idMap.clear();
            m_timeMap.clear();
            m_shutdown = true;
            m_condVariable.notify_one();
        }

        m_thread.join();
    }

    TimeoutDispatcher::TimeoutId TimeoutDispatcher::Dispatch(std::shared_ptr<Napi::FunctionReference> function, std::chrono::milliseconds delay)
    {
        if (delay.count() < 0)
        {
            delay = std::chrono::milliseconds{0};
        }

        const auto id = NextTimeoutId();

        m_runtime.Dispatch([this, function = std::move(function), delay, id](Napi::Env) {
            std::unique_lock<std::mutex> lk{m_mutex};

            const auto earliestTime = m_timeMap.empty() ? TimePoint::max()
                                                        : m_timeMap.cbegin()->second->time;
            const auto time = Now() + delay;
            const auto result = m_idMap.insert({id, std::make_unique<Timeout>(id, std::move(function), time)});
            m_timeMap.insert({time, result.first->second.get()});

            if (time <= earliestTime)
            {
                m_condVariable.notify_one();
            }
        });

        return id;
    }

    void TimeoutDispatcher::Clear(TimeoutId id)
    {
        m_runtime.Dispatch([this, id](Napi::Env){
            std::unique_lock<std::mutex> lk{m_mutex};
            const auto itId = m_idMap.find(id);
            if (itId != m_idMap.end())
            {
                const auto& timeout = itId->second;
                const auto timeRange = m_timeMap.equal_range(timeout->time);

                assert(timeRange.first != m_timeMap.end() && "m_idMap and m_timeMap are out of sync");

                for (auto itTime = timeRange.first; itTime != timeRange.second; itTime++)
                {
                    if (itTime->second->id == id)
                    {
                        m_timeMap.erase(itTime);
                        break;
                    }
                }

                m_idMap.erase(itId);
            }
        });
    }

    TimeoutDispatcher::TimeoutId TimeoutDispatcher::NextTimeoutId()
    {
        std::unique_lock<std::mutex> lk{m_mutex};

        while (true)
        {
            ++m_lastTimeoutId;

            if (m_lastTimeoutId <= 0)
            {
                m_lastTimeoutId = 1;
            }

            if (m_idMap.find(m_lastTimeoutId) == m_idMap.end())
            {
                return m_lastTimeoutId;
            }
        }
    }

    void TimeoutDispatcher::ThreadFunction()
    {
        while (!m_shutdown)
        {
            std::unique_lock<std::mutex> lk{m_mutex};
            TimePoint nextTimePoint{};

            while (!m_timeMap.empty())
            {
                nextTimePoint = m_timeMap.begin()->second->time;
                if (nextTimePoint <= Now())
                {
                    break;
                }

                m_condVariable.wait_until(lk, nextTimePoint);
            }

            while (!m_timeMap.empty() && m_timeMap.begin()->second->time == nextTimePoint)
            {
                const auto* timeout = m_timeMap.begin()->second;
                CallFunction(timeout->function);
                m_timeMap.erase(m_timeMap.begin());
                m_idMap.erase(timeout->id);
            }

            while (!m_shutdown && m_timeMap.empty())
            {
                m_condVariable.wait(lk);
            }
        }
    }

    void TimeoutDispatcher::CallFunction(std::shared_ptr<Napi::FunctionReference> function)
    {
        m_runtime.Dispatch([function = std::move(function)](Napi::Env)
            { function->Call({}); });
    }
}

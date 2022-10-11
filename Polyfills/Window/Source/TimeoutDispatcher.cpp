#include "TimeoutDispatcher.h"

#include <cassert>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        using TimePoint = std::chrono::time_point<std::chrono::steady_clock, std::chrono::milliseconds>;

        TimePoint Now()
        {
            return std::chrono::time_point_cast<std::chrono::milliseconds, std::chrono::steady_clock>(std::chrono::steady_clock::now());
        }
    }

    struct Timeout
    {
        TimeoutId id;

        // Make this non-shared when JsRuntime::Dispatch supports it.
        std::shared_ptr<Napi::FunctionReference> function;

        TimePoint time;

        Timeout(TimeoutId id, Napi::Function function, TimePoint time)
            : id{id}
            , function{std::make_shared<Napi::FunctionReference>(Napi::Persistent(function))}
            , time{time}
        {
        }

        Timeout(const Timeout&) = delete;
        Timeout(Timeout&&) = delete;
    };

    TimeoutDispatcher::TimeoutDispatcher(Babylon::JsRuntime& runtime)
        : m_runtime(runtime)
        , m_thread{&TimeoutDispatcher::ThreadFunction, this}
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

    TimeoutId TimeoutDispatcher::Dispatch(Napi::Function function, std::chrono::milliseconds delay)
    {
        if (delay.count() < 0)
        {
            delay = std::chrono::milliseconds{0};
        }

        std::unique_lock<std::mutex> lk{m_mutex};

        const auto earliestTime = m_timeMap.empty() ? TimePoint::max()
                                                    : m_timeMap.cbegin()->second->time;
        const auto id = NextTimeoutId();
        const auto time = Now() + delay;
        m_idMap.insert({id, std::make_unique<Timeout>(id, std::move(function), time)});
        m_timeMap.insert({time, m_idMap[id].get()});

        if (time <= earliestTime)
        {
            m_runtime.Dispatch([this](Napi::Env){ m_condVariable.notify_one(); });
        }

        return id;
    }

    void TimeoutDispatcher::Clear(TimeoutId id)
    {
        std::unique_lock<std::mutex> lk{m_mutex};
        const auto idIterator = m_idMap.find(id);
        if (idIterator != m_idMap.end())
        {
            const auto& timeout = idIterator->second;

            assert(m_timeMap.lower_bound(timeout->time) != m_timeMap.end() && "m_idMap and m_timeMap are out of sync");

            for (auto i = m_timeMap.lower_bound(timeout->time); i->first == timeout->time; i++)
            {
                if (i->second->id == id)
                {
                    m_timeMap.erase(i);
                    break;
                }
            }

            m_idMap.erase(idIterator);
        }
    }

    TimeoutId TimeoutDispatcher::NextTimeoutId()
    {
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
        TimePoint nextTimePoint{};
        while (!m_shutdown)
        {
            std::unique_lock<std::mutex> lk{m_mutex};

            while (!m_timeMap.empty() && Now() < (nextTimePoint = m_timeMap.begin()->second->time))
            {
                m_condVariable.wait_until(lk, nextTimePoint);
            }

            for (auto i = m_timeMap.begin(); i != m_timeMap.end() && i->second->time == nextTimePoint; i = m_timeMap.begin())
            {
                const auto *timeout = i->second;
                CallFunction(timeout->function);
                m_timeMap.erase(i);
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

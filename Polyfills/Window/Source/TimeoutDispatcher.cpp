#include "TimeoutDispatcher.h"

#include <assert.h>

namespace Babylon::Polyfills::Internal
{
    TimePoint Now()
    {
        return std::chrono::time_point_cast<std::chrono::milliseconds, std::chrono::steady_clock>(std::chrono::steady_clock::now());
    }

    struct Timeout
    {
        TimeoutId id;
        std::shared_ptr<Napi::FunctionReference> function;
        TimePoint time;

        Timeout(TimeoutId id, std::shared_ptr<Napi::FunctionReference> func, TimePoint time)
            : id{id}
            , function{std::move(func)}
            , time{time}
        {
        }
    };

    TimeoutDispatcher::TimeoutDispatcher(Babylon::JsRuntime& runtime)
        : m_runtime(runtime)
        , m_thread{&TimeoutDispatcher::WaitThenCallProc, this}
    {
    }

    TimeoutDispatcher::~TimeoutDispatcher()
    {
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            m_idMap.clear();
            m_timeMap.clear();
            m_shutdown = true;
            m_condVariable.notify_one();
        }

        if (m_thread.joinable())
        {
            m_thread.join();
        }
    }

    TimeoutId TimeoutDispatcher::Dispatch(std::shared_ptr<Napi::FunctionReference> func, Milliseconds delay)
    {
        if (delay.count() <= 0)
        {
            CallFunction(std::move(func));
            return 0;
        }

        const auto now = Now();
        std::unique_lock<std::mutex> lk(m_mutex);
        const auto earliestTime =
            m_timeMap.empty()
            ? TimePoint::max()
            : m_timeMap.cbegin()->second.front()->time;

        const auto id = NextTimeoutId();
        const auto time = now + delay;
        m_idMap.insert({id, std::make_unique<Timeout>(id, std::move(func), time)});
        m_timeMap[time].push_back(m_idMap[id].get());

        if (time <= earliestTime)
        {
            m_condVariable.notify_one();
        }

        return id;
    }

    void TimeoutDispatcher::Clear(TimeoutId id)
    {
        std::unique_lock<std::mutex> lk(m_mutex);
        const auto idIterator = m_idMap.find(id);
        if (idIterator != m_idMap.end())
        {
            const auto& time = idIterator->second->time;
            const auto& timeoutId = idIterator->second->id;

            const auto timeIterator = m_timeMap.find(time);
            assert(timeIterator != m_timeMap.end() && "m_idMap and m_timeMap are out of sync");

            auto& timeouts = timeIterator->second;
            for (auto i = timeouts.begin(); i != timeouts.end(); i++)
            {
                if ((*i)->id == timeoutId)
                {
                    timeouts.erase(i);
                    break;
                }
            }
            if (timeouts.empty())
            {
                m_timeMap.erase(timeIterator);
            }

            m_idMap.erase(idIterator);
        }
    }

    void TimeoutDispatcher::WaitThenCallProc()
    {
        TimePoint nextTimePoint{};
        while (!m_shutdown)
        {
            std::unique_lock<std::mutex> lk(m_mutex);
            while (!m_timeMap.empty() && Now() < (nextTimePoint = m_timeMap.begin()->second.front()->time))
            {
                m_condVariable.wait_until(lk, nextTimePoint);
            }

            if (!m_timeMap.empty())
            {
                auto& timeouts = m_timeMap.begin()->second;
                while (!timeouts.empty())
                {
                    auto& timeout = timeouts.front();
                    CallFunction(timeout->function);
                    m_idMap.erase(timeout->id);
                    timeouts.pop_front();
                }
                m_timeMap.erase(m_timeMap.begin());
            }

            while (!m_shutdown && m_timeMap.empty())
            {
                m_condVariable.wait(lk);
            }
        }
    }

    void TimeoutDispatcher::CallFunction(std::shared_ptr<Napi::FunctionReference> func)
    {
        m_runtime.Dispatch([func = std::move(func)](Napi::Env)
            { func->Call({}); });
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
}

#include "TimeoutDispatcher.h"

#include <cassert>
#include <optional>

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

        // Distinguishes this timeout from a later one that happens to reuse the
        // same id, so an in-flight callback can never re-arm its replacement.
        uint64_t sequence;

        // Make this non-shared when JsRuntime::Dispatch supports it.
        std::shared_ptr<Napi::FunctionReference> function;

        TimePoint time;

        std::optional<std::chrono::milliseconds> interval;

        Timeout(TimeoutId id, uint64_t sequence, std::shared_ptr<Napi::FunctionReference> function, TimePoint time, std::optional<std::chrono::milliseconds> interval)
            : id{id}
            , sequence{sequence}
            , function{std::move(function)}
            , time{time}
            , interval{interval}
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
            std::unique_lock<std::recursive_mutex> lk{m_mutex};
            m_idMap.clear();
            m_timeMap.clear();
        }

        m_shutdown = true;
        m_condVariable.notify_one();
        m_thread.join();
    }

    TimeoutDispatcher::TimeoutId TimeoutDispatcher::Dispatch(std::shared_ptr<Napi::FunctionReference> function, std::chrono::milliseconds delay, bool repeat)
    {
        return DispatchImpl(function, delay, repeat, 0);
    }

    TimeoutDispatcher::TimeoutId TimeoutDispatcher::DispatchImpl(std::shared_ptr<Napi::FunctionReference> function, std::chrono::milliseconds delay, bool repeat, TimeoutId id)
    {
        if (delay.count() < 0)
        {
            delay = std::chrono::milliseconds{0};
        }

        std::unique_lock<std::recursive_mutex> lk{m_mutex};

        if (id == 0)
        {
            id = NextTimeoutId();
        }
        const auto earliestTime = m_timeMap.empty() ? TimePoint::max() : m_timeMap.cbegin()->second->time;
        const auto time = Now() + delay;
        const auto result = m_idMap.insert({id, std::make_unique<Timeout>(id, ++m_lastSequence, std::move(function), time, repeat ? std::make_optional<std::chrono::milliseconds>(delay) : std::nullopt)});
        m_timeMap.insert({time, result.first->second.get()});

        if (time <= earliestTime)
        {
            m_condVariable.notify_one();
        }

        return id;
    }

    void TimeoutDispatcher::Clear(TimeoutId id)
    {
        std::unique_lock<std::recursive_mutex> lk{m_mutex};
        const auto itId = m_idMap.find(id);
        if (itId != m_idMap.end())
        {
            const auto& timeout = itId->second;
            const auto timeRange = m_timeMap.equal_range(timeout->time);

            // Remove any pending entries that have not yet been dispatched.
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
    }

    TimeoutDispatcher::TimeoutId TimeoutDispatcher::NextTimeoutId()
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
        while (!m_shutdown)
        {
            std::unique_lock<std::recursive_mutex> lk{m_mutex};
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
                const auto id = m_timeMap.begin()->second->id;
                const auto sequence = m_timeMap.begin()->second->sequence;
                m_timeMap.erase(m_timeMap.begin());

                // Repeating timeouts are deliberately NOT re-armed here. They are
                // re-armed on the JS thread once the callback has actually run, so
                // that at most one invocation of a given interval is ever queued.
                // Re-arming here instead would let this thread -- which never waits
                // while a due timeout exists -- spin and enqueue callbacks far
                // faster than the JS thread can drain them. The resulting unbounded
                // backlog starves every other item on the JS dispatch queue: other
                // timers, and native async completions such as shader compilation.
                CallFunction(id, sequence);
            }

            while (!m_shutdown && m_timeMap.empty())
            {
                m_condVariable.wait(lk);
            }
        }
    }

    void TimeoutDispatcher::CallFunction(TimeoutId id, uint64_t sequence)
    {
        m_runtime.Dispatch([id, sequence, this](Napi::Env) {
            std::shared_ptr<Napi::FunctionReference> function{};
            std::optional<std::chrono::milliseconds> interval{};
            TimePoint scheduledTime{};
            {
                std::unique_lock<std::recursive_mutex> lk{m_mutex};
                const auto it = m_idMap.find(id);
                if (it == m_idMap.end() || it->second->sequence != sequence)
                {
                    // Cleared before the callback could run, or the id has since
                    // been reused by an unrelated timeout.
                    return;
                }

                interval = it->second->interval;
                scheduledTime = it->second->time;

                if (interval.has_value())
                {
                    function = it->second->function;
                }
                else
                {
                    const auto timeout = std::move(m_idMap.extract(id).mapped());
                    function = std::move(timeout->function);
                }
            }

            if (function)
            {
                try
                {
                    function->Call({});
                }
                catch (const Napi::Error& error)
                {
                    // A throwing tick must not silently stop the interval, which
                    // is both the pre-existing behavior and what browsers do.
                    // Re-arm first, then re-raise the error as a pending JS
                    // exception so JsRuntime::Dispatch still surfaces it.
                    if (interval.has_value())
                    {
                        Rearm(id, sequence, scheduledTime, *interval);
                    }

                    error.ThrowAsJavaScriptException();
                    return;
                }
            }

            if (interval.has_value())
            {
                Rearm(id, sequence, scheduledTime, *interval);
            }
        });
    }

    // Re-arms a repeating timeout. Called on the JS thread once the callback has
    // returned, so a repeating timeout can never have more than one invocation
    // queued at a time.
    void TimeoutDispatcher::Rearm(TimeoutId id, uint64_t sequence, TimePoint scheduledTime, std::chrono::milliseconds interval)
    {
        std::unique_lock<std::recursive_mutex> lk{m_mutex};

        const auto it = m_idMap.find(id);
        if (it == m_idMap.end() || it->second->sequence != sequence)
        {
            // Cleared from within its own callback, or the id has since been
            // reused by an unrelated timeout.
            return;
        }

        // Anchor the next deadline to the previous scheduled time so that a long
        // running callback does not accumulate drift, but never schedule into the
        // past.
        const auto now = Now();
        auto nextTime = scheduledTime + interval;
        if (nextTime < now)
        {
            nextTime = now;
        }

        const auto earliestTime = m_timeMap.empty() ? TimePoint::max() : m_timeMap.cbegin()->second->time;
        it->second->time = nextTime;
        m_timeMap.insert({nextTime, it->second.get()});

        if (nextTime <= earliestTime)
        {
            // The timer thread parks while m_timeMap is empty, which is the case
            // whenever this timeout was the only one pending.
            m_condVariable.notify_one();
        }
    }
}

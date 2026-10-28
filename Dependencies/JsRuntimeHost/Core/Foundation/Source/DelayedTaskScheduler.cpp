#include "DelayedTaskScheduler.h"

#include <condition_variable>
#include <limits>
#include <map>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <unordered_map>
#include <utility>

namespace Babylon::Internal
{
    namespace
    {
        constexpr auto JS_DELAYED_TASK_SCHEDULER_NAME = "_BabylonDelayedTaskScheduler";

        DelayedTaskScheduler::TimePoint Now()
        {
            return std::chrono::time_point_cast<std::chrono::microseconds, std::chrono::steady_clock>(std::chrono::steady_clock::now());
        }
    }

    class DelayedTaskScheduler::Impl
    {
    public:
        explicit Impl(Id lastId)
            : m_lastId{lastId}
            , m_thread{&Impl::ThreadFunction, this}
        {
        }

        ~Impl()
        {
            Shutdown();
        }

        Id Schedule(TimePoint when, Callback callback)
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            if (m_shutdown)
            {
                throw std::runtime_error{"DelayedTaskScheduler: Schedule after shutdown"};
            }

            const auto earliestTime = m_timeMap.empty() ? TimePoint::max() : m_timeMap.begin()->first;
            const auto id = NextId();
            auto item = std::make_unique<Item>(id, when, std::move(callback));
            Item* const rawItem = item.get();
            const auto [it, inserted] = m_idMap.try_emplace(id, std::move(item));
            if (!inserted)
            {
                throw std::logic_error{"DelayedTaskScheduler: NextId returned a duplicate id"};
            }

            m_timeMap.emplace(when, rawItem);
            if (when <= earliestTime)
            {
                m_conditionVariable.notify_one();
            }

            return id;
        }

        void Cancel(Id id)
        {
            std::unique_lock<std::mutex> lock{m_mutex};
            const auto idIt = m_idMap.find(id);
            if (idIt == m_idMap.end())
            {
                return;
            }

            const bool wasEarliest = !m_timeMap.empty() && m_timeMap.begin()->second == idIt->second.get();
            const auto timeRange = m_timeMap.equal_range(idIt->second->time);
            for (auto timeIt = timeRange.first; timeIt != timeRange.second; ++timeIt)
            {
                if (timeIt->second == idIt->second.get())
                {
                    m_timeMap.erase(timeIt);
                    break;
                }
            }

            m_idMap.erase(idIt);
            if (wasEarliest)
            {
                m_conditionVariable.notify_one();
            }
        }

        void Shutdown()
        {
            std::scoped_lock shutdownLock{m_shutdownMutex};
            {
                std::unique_lock<std::mutex> lock{m_mutex};
                if (m_shutdown)
                {
                    return;
                }

                m_shutdown = true;
                m_idMap.clear();
                m_timeMap.clear();
            }

            m_conditionVariable.notify_one();
            m_thread.join();
        }

    private:
        struct Item
        {
            Item(Id id, TimePoint time, Callback callback)
                : id{id}
                , time{time}
                , callback{std::move(callback)}
            {
            }

            Id id;
            TimePoint time;
            Callback callback;
        };

        Id NextId()
        {
            while (true)
            {
                m_lastId = m_lastId == std::numeric_limits<Id>::max() ? 1 : m_lastId + 1;

                if (m_idMap.find(m_lastId) == m_idMap.end())
                {
                    return m_lastId;
                }
            }
        }

        void ThreadFunction()
        {
            while (true)
            {
                Callback callback;
                {
                    std::unique_lock<std::mutex> lock{m_mutex};
                    while (!m_shutdown && m_timeMap.empty())
                    {
                        m_conditionVariable.wait(lock);
                    }

                    if (m_shutdown)
                    {
                        return;
                    }

                    const auto nextTime = m_timeMap.begin()->first;
                    if (nextTime > Now())
                    {
                        m_conditionVariable.wait_until(lock, nextTime);
                        continue;
                    }

                    Item* const item = m_timeMap.begin()->second;
                    callback = std::move(item->callback);
                    m_idMap.erase(item->id);
                    m_timeMap.erase(m_timeMap.begin());
                }

                if (callback)
                {
                    callback();
                }
            }
        }

        std::mutex m_mutex;
        std::mutex m_shutdownMutex;
        std::condition_variable m_conditionVariable;
        Id m_lastId{};
        std::unordered_map<Id, std::unique_ptr<Item>> m_idMap;
        std::multimap<TimePoint, Item*> m_timeMap;
        bool m_shutdown{};
        std::thread m_thread;
    };

    DelayedTaskScheduler::DelayedTaskScheduler()
        : DelayedTaskScheduler{0}
    {
    }

    DelayedTaskScheduler::DelayedTaskScheduler(Id lastId)
        : m_impl{std::make_unique<Impl>(lastId)}
    {
    }

    DelayedTaskScheduler::~DelayedTaskScheduler() = default;

    void DelayedTaskScheduler::SetForJavaScript(Napi::Env env, DelayedTaskScheduler& scheduler)
    {
        env.Global().Set(JS_DELAYED_TASK_SCHEDULER_NAME, Napi::External<DelayedTaskScheduler>::New(env, &scheduler));
    }

    void DelayedTaskScheduler::ClearFromJavaScript(Napi::Env env)
    {
        env.Global().Set(JS_DELAYED_TASK_SCHEDULER_NAME, env.Undefined());
    }

    DelayedTaskScheduler* DelayedTaskScheduler::GetFromJavaScript(Napi::Env env)
    {
        const auto value = env.Global().Get(JS_DELAYED_TASK_SCHEDULER_NAME);
        return value.IsUndefined() ? nullptr : value.As<Napi::External<DelayedTaskScheduler>>().Data();
    }

    DelayedTaskScheduler::Id DelayedTaskScheduler::Schedule(TimePoint when, Callback callback)
    {
        return m_impl->Schedule(when, std::move(callback));
    }

    DelayedTaskScheduler::Id DelayedTaskScheduler::Schedule(std::chrono::milliseconds delay, Callback callback)
    {
        if (delay.count() < 0)
        {
            delay = std::chrono::milliseconds{0};
        }

        return Schedule(Now() + delay, std::move(callback));
    }

    void DelayedTaskScheduler::Cancel(Id id)
    {
        m_impl->Cancel(id);
    }

    void DelayedTaskScheduler::Shutdown()
    {
        m_impl->Shutdown();
    }
}

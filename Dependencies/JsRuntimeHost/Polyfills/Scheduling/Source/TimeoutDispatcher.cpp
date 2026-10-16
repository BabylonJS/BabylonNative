#include "TimeoutDispatcher.h"

#include "DelayedTaskScheduler.h"

#include <limits>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    using Babylon::Internal::DelayedTaskScheduler;

    namespace
    {
        DelayedTaskScheduler::TimePoint Now()
        {
            return std::chrono::time_point_cast<std::chrono::microseconds, std::chrono::steady_clock>(std::chrono::steady_clock::now());
        }
    }

    struct TimeoutDispatcher::Timeout
    {
        Timeout(TimeoutId id, uint64_t sequence, std::shared_ptr<Napi::FunctionReference> function, DelayedTaskScheduler::TimePoint time, std::optional<std::chrono::milliseconds> interval)
            : id{id}
            , sequence{sequence}
            , function{std::move(function)}
            , time{time}
            , interval{interval}
        {
        }

        TimeoutId id;
        uint64_t sequence;
        std::shared_ptr<Napi::FunctionReference> function;
        DelayedTaskScheduler::TimePoint time;
        std::optional<std::chrono::milliseconds> interval;
        DelayedTaskScheduler::Id scheduleId{};
    };

    struct TimeoutDispatcher::State
    {
        State(Napi::Env env, Babylon::JsRuntime& runtime, TimeoutId lastTimeoutId)
            : runtime{&runtime}
            , lastTimeoutId{lastTimeoutId}
        {
            scheduler = DelayedTaskScheduler::GetFromJavaScript(env);
            if (scheduler == nullptr)
            {
                ownedScheduler = std::make_unique<DelayedTaskScheduler>();
                scheduler = ownedScheduler.get();
            }
        }

        TimeoutId NextTimeoutId()
        {
            while (true)
            {
                lastTimeoutId = lastTimeoutId == std::numeric_limits<TimeoutId>::max() ? 1 : lastTimeoutId + 1;

                if (timeouts.find(lastTimeoutId) == timeouts.end())
                {
                    return lastTimeoutId;
                }
            }
        }

        void CallFunction(const std::shared_ptr<State>& self, TimeoutId id, uint64_t sequence);
        void Rearm(const std::shared_ptr<State>& self, TimeoutId id, uint64_t sequence, DelayedTaskScheduler::TimePoint scheduledTime, std::chrono::milliseconds interval);

        std::recursive_mutex mutex;
        Babylon::JsRuntime* runtime;
        std::unique_ptr<DelayedTaskScheduler> ownedScheduler;
        DelayedTaskScheduler* scheduler;
        TimeoutId lastTimeoutId{};
        uint64_t lastSequence{};
        std::unordered_map<TimeoutId, std::unique_ptr<Timeout>> timeouts;
        bool active{true};
    };

    void TimeoutDispatcher::State::CallFunction(const std::shared_ptr<State>& self, TimeoutId id, uint64_t sequence)
    {
        std::scoped_lock lock{mutex};
        const auto timeoutIt = timeouts.find(id);
        if (!active || timeoutIt == timeouts.end() || timeoutIt->second->sequence != sequence)
        {
            return;
        }

        runtime->Dispatch([self, id, sequence](Napi::Env) {
            std::shared_ptr<Napi::FunctionReference> function;
            std::optional<std::chrono::milliseconds> interval;
            DelayedTaskScheduler::TimePoint scheduledTime;
            {
                std::scoped_lock callbackLock{self->mutex};
                const auto callbackIt = self->timeouts.find(id);
                if (!self->active || callbackIt == self->timeouts.end() || callbackIt->second->sequence != sequence)
                {
                    return;
                }

                interval = callbackIt->second->interval;
                scheduledTime = callbackIt->second->time;
                if (interval.has_value())
                {
                    function = callbackIt->second->function;
                }
                else
                {
                    auto timeout = std::move(self->timeouts.extract(id).mapped());
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
                    if (interval.has_value())
                    {
                        self->Rearm(self, id, sequence, scheduledTime, *interval);
                    }

                    error.ThrowAsJavaScriptException();
                    return;
                }
            }

            if (interval.has_value())
            {
                self->Rearm(self, id, sequence, scheduledTime, *interval);
            }
        });
    }

    void TimeoutDispatcher::State::Rearm(const std::shared_ptr<State>& self, TimeoutId id, uint64_t sequence, DelayedTaskScheduler::TimePoint scheduledTime, std::chrono::milliseconds interval)
    {
        std::scoped_lock lock{mutex};
        const auto timeoutIt = timeouts.find(id);
        if (!active || timeoutIt == timeouts.end() || timeoutIt->second->sequence != sequence)
        {
            return;
        }

        const auto now = Now();
        auto nextTime = scheduledTime + interval;
        if (nextTime < now)
        {
            nextTime = now;
        }

        timeoutIt->second->time = nextTime;
        timeoutIt->second->scheduleId = scheduler->Schedule(nextTime, [self, id, sequence]() {
            self->CallFunction(self, id, sequence);
        });
    }

    TimeoutDispatcher::TimeoutDispatcher(Napi::Env env, Babylon::JsRuntime& runtime)
        : TimeoutDispatcher{env, runtime, 0}
    {
    }

    TimeoutDispatcher::TimeoutDispatcher(Napi::Env env, Babylon::JsRuntime& runtime, TimeoutId lastTimeoutId)
        : m_state{std::make_shared<State>(env, runtime, lastTimeoutId)}
    {
    }

    TimeoutDispatcher::~TimeoutDispatcher()
    {
        std::vector<DelayedTaskScheduler::Id> scheduleIds;
        DelayedTaskScheduler* ownedScheduler{};
        {
            std::scoped_lock lock{m_state->mutex};
            m_state->active = false;
            m_state->runtime = nullptr;
            scheduleIds.reserve(m_state->timeouts.size());
            for (const auto& [id, timeout] : m_state->timeouts)
            {
                scheduleIds.push_back(timeout->scheduleId);
            }
            m_state->timeouts.clear();
            ownedScheduler = m_state->ownedScheduler.get();
        }

        for (const auto scheduleId : scheduleIds)
        {
            m_state->scheduler->Cancel(scheduleId);
        }

        if (ownedScheduler != nullptr)
        {
            ownedScheduler->Shutdown();
        }
    }

    TimeoutDispatcher::TimeoutId TimeoutDispatcher::Dispatch(std::shared_ptr<Napi::FunctionReference> function, std::chrono::milliseconds delay, bool repeat)
    {
        if (delay.count() < 0)
        {
            delay = std::chrono::milliseconds{0};
        }

        std::scoped_lock lock{m_state->mutex};
        const auto id = m_state->NextTimeoutId();
        const auto sequence = ++m_state->lastSequence;
        const auto time = Now() + delay;
        auto timeout = std::make_unique<Timeout>(
            id,
            sequence,
            std::move(function),
            time,
            repeat ? std::make_optional(delay) : std::nullopt);
        const auto [timeoutIt, inserted] = m_state->timeouts.try_emplace(id, std::move(timeout));
        if (!inserted)
        {
            throw std::logic_error{"TimeoutDispatcher: NextTimeoutId returned a duplicate id"};
        }

        const auto state = m_state;
        timeoutIt->second->scheduleId = m_state->scheduler->Schedule(time, [state, id, sequence]() {
            state->CallFunction(state, id, sequence);
        });
        return id;
    }

    void TimeoutDispatcher::Clear(TimeoutId id)
    {
        DelayedTaskScheduler::Id scheduleId{};
        {
            std::scoped_lock lock{m_state->mutex};
            const auto timeoutIt = m_state->timeouts.find(id);
            if (timeoutIt == m_state->timeouts.end())
            {
                return;
            }

            scheduleId = timeoutIt->second->scheduleId;
            m_state->timeouts.erase(timeoutIt);
        }

        m_state->scheduler->Cancel(scheduleId);
    }
}

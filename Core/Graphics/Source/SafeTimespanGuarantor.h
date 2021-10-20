#pragma once

#include "continuation_scheduler.h"

#include <arcana/threading/task.h>

#include <gsl/gsl>

#include <condition_variable>
#include <future>
#include <mutex>

namespace Babylon
{
    template<typename ReturnT, typename ErrorT>
    void blocking_await(arcana::task<ReturnT, ErrorT>& task)
    {
        std::promise<void> promise{};
        auto future = promise.get_future();
        task.then(arcana::inline_scheduler, arcana::cancellation::none(), [&promise]()
        { 
            promise.set_value();
        });
        future.wait();
    }

    class SafeTimespanGuarantor
    {
    public:
        void BeginSafeTimespan();
        void NonblockingEndSafeTimespan();
        arcana::task<void, std::exception_ptr> EndSafeTimespanAsync();

        continuation_scheduler<>& BeginScheduler()
        {
            return m_beginScheduler;
        }

        continuation_scheduler<>& EndScheduler()
        {
            return m_endScheduler;
        }

        using SafetyGuarantee = gsl::final_action<std::function<void()>>;
        SafetyGuarantee GetSafetyGuarantee();

    private:
        enum class State
        {
            Open,
            Closing,
            Closed
        };

        State m_state{State::Open};
        uint32_t m_count{};
        std::mutex m_mutex{};
        std::condition_variable m_condition_variable{};
        tickable_continuation_scheduler<SafeTimespanGuarantor> m_beginScheduler{};
        tickable_continuation_scheduler<SafeTimespanGuarantor> m_endScheduler{};
    };
}

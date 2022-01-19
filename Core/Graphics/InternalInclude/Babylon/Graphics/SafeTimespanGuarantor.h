#pragma once

#include "continuation_scheduler.h"

#include <arcana/threading/cancellation.h>
#include <arcana/threading/task.h>

#include <gsl/gsl>

#include <condition_variable>
#include <mutex>
#include <optional>

namespace Babylon::Graphics
{
    class SafeTimespanGuarantor
    {
    public:
        SafeTimespanGuarantor(std::optional<arcana::cancellation_source>&);

        continuation_scheduler<>& OpenScheduler()
        {
            return m_openDispatcher.scheduler();
        }

        continuation_scheduler<>& CloseScheduler()
        {
            return m_closeDispatcher.scheduler();
        }

        using SafetyGuarantee = gsl::final_action<std::function<void()>>;
        SafetyGuarantee GetSafetyGuarantee();
        
        void Open();
        void RequestClose();
        void Lock();
        void Unlock();

    private:
        enum class State
        {
            Open,
            Closing,
            Closed,
            Locked
        };

        std::optional<arcana::cancellation_source>& m_cancellation;
        State m_state{State::Locked};
        uint32_t m_count{};
        std::mutex m_mutex{};
        std::condition_variable m_condition_variable{};
        continuation_dispatcher<> m_openDispatcher{};
        continuation_dispatcher<> m_closeDispatcher{};
    };
}

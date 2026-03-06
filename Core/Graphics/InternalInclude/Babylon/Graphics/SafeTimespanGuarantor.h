#pragma once

#include "continuation_scheduler.h"

#include <arcana/threading/cancellation.h>
#include <arcana/threading/task.h>

#include <gsl/gsl>

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

        void Open();
        void RequestClose();

    private:
        enum class State
        {
            Open,
            Closed
        };

        std::optional<arcana::cancellation_source>& m_cancellation;
        State m_state{State::Closed};
        std::mutex m_mutex{};
        continuation_dispatcher<> m_openDispatcher{};
        continuation_dispatcher<> m_closeDispatcher{};
    };
}

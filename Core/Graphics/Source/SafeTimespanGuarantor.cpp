#include "SafeTimespanGuarantor.h"

namespace Babylon
{
    SafeTimespanGuarantor::SafeTimespanGuarantor()
        : m_affinity{std::this_thread::get_id()}
    {
    }

    void SafeTimespanGuarantor::BeginSafeTimespan()
    {
        if (!m_affinity.check())
        {
            throw std::runtime_error{"BeginSafeTimespan must be called from the thread on which the SafeTimespanGuarantor was constructed."};
        }

        {
            std::scoped_lock lock{m_mutex};
            m_state = State::Open;
        }

        m_condition_variable.notify_all();
        std::this_thread::yield();

        m_beginScheduler.tick(arcana::cancellation::none());
    }

    void SafeTimespanGuarantor::NonblockingEndSafeTimespan()
    {
        if (!m_affinity.check())
        {
            throw std::runtime_error{ "EndSafeTimespan must be called from the thread on which the SafeTimespanGuarantor was constructed." };
        }

        std::scoped_lock lock{m_mutex};
        if (m_count == 0)
        {
            m_state = State::Closed;
            m_endScheduler.tick(arcana::cancellation::none());
        }
        else
        {
            m_state = State::Closing;
        }
    }

    arcana::task<void, std::exception_ptr> SafeTimespanGuarantor::EndSafeTimespanAsync()
    {
        auto task = arcana::make_task(m_endScheduler, arcana::cancellation::none(), []() {});
        NonblockingEndSafeTimespan();
        return task;
    }

    SafeTimespanGuarantor::SafetyGuarantee SafeTimespanGuarantor::GetSafetyGuarantee()
    {
        std::unique_lock lock{m_mutex};
        if (m_state == State::Closed)
        {
            m_condition_variable.wait(lock, [this]() { return m_state != State::Closed; });
        }
        m_count++;

        return gsl::finally(std::function<void()>{ [this]
        {
            std::scoped_lock lock{m_mutex};
            if (--m_count == 0 && m_state == State::Closing)
            {
                m_state = State::Closed;
                m_endScheduler.tick(arcana::cancellation::none());
            }
        }});
    }
}

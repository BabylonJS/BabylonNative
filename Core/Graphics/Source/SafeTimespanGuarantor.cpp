#include "SafeTimespanGuarantor.h"

namespace Babylon
{
    void SafeTimespanGuarantor::BeginSafeTimespan()
    {
        {
            std::scoped_lock lock{m_mutex};
            if (m_state != State::Closed)
            {
                throw std::runtime_error{"Safe timespan cannot begin if guarantor state is not closed"};
            }
            m_state = State::Open;
        }

        m_condition_variable.notify_all();
        std::this_thread::yield();

        m_beginScheduler.tick(arcana::cancellation::none());
    }

    void SafeTimespanGuarantor::NonblockingEndSafeTimespan()
    {
        std::scoped_lock lock{m_mutex};
        if (m_state != State::Open)
        {
            throw std::runtime_error{"Safe timespan cannot end if guarantor state is not open"};
        }
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

    void SafeTimespanGuarantor::Lock()
    {
        std::scoped_lock lock{m_mutex};
        if (m_state != State::Closed)
        {
            throw std::runtime_error{"SafeTimespanGuarantor can only be locked from a closed state"};
        }
        m_state = State::Locked;
    }

    void SafeTimespanGuarantor::Unlock()
    {
        std::scoped_lock lock{m_mutex};
        if (m_state != State::Locked)
        {
            throw std::runtime_error{"SafeTimespanGuarantor can only be unlocked if it was locked"};
        }
        m_state = State::Closed;
    }

    SafeTimespanGuarantor::SafetyGuarantee SafeTimespanGuarantor::GetSafetyGuarantee()
    {
        std::unique_lock lock{m_mutex};
        if (m_state == State::Closed || m_state == State::Locked)
        {
            m_condition_variable.wait(lock, [this]() { return m_state != State::Closed && m_state != State::Locked; });
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

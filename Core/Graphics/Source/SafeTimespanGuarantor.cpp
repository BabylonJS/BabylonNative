#include "SafeTimespanGuarantor.h"

namespace Babylon
{
    SafeTimespanGuarantor::SafeTimespanGuarantor(std::optional<arcana::cancellation_source>& cancellation)
        : m_cancellation{cancellation}
    {
    }

    void SafeTimespanGuarantor::Open()
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

        m_openDispatcher.tick(*m_cancellation);
    }

    void SafeTimespanGuarantor::RequestClose()
    {
        std::scoped_lock lock{m_mutex};
        if (m_state != State::Open)
        {
            throw std::runtime_error{"Safe timespan cannot end if guarantor state is not open"};
        }
        if (m_count == 0)
        {
            m_state = State::Closed;
            m_closeDispatcher.tick(*m_cancellation);
        }
        else
        {
            m_state = State::Closing;
        }
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
                m_closeDispatcher.tick(*m_cancellation);
            }
        }});
    }
}

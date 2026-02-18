#include "SafeTimespanGuarantor.h"

namespace Babylon::Graphics
{
    SafeTimespanGuarantor::SafeTimespanGuarantor(CancellationSourceGetter getCancellationSource)
        : m_getCancellationSource{std::move(getCancellationSource)}
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

        auto cancellationSource = TryGetCancellationSource();
        if (cancellationSource)
        {
            m_openDispatcher.tick(*cancellationSource);
        }
    }

    void SafeTimespanGuarantor::RequestClose()
    {
        bool shouldTickCloseDispatcher{};

        {
            std::scoped_lock lock{m_mutex};
            if (m_state != State::Open)
            {
                throw std::runtime_error{"Safe timespan cannot end if guarantor state is not open"};
            }
            if (m_count == 0)
            {
                m_state = State::Closed;
                shouldTickCloseDispatcher = true;
            }
            else
            {
                m_state = State::Closing;
            }
        }

        if (!shouldTickCloseDispatcher)
        {
            return;
        }

        auto cancellationSource = TryGetCancellationSource();
        if (cancellationSource)
        {
            m_closeDispatcher.tick(*cancellationSource);
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

        return gsl::finally(std::function<void()>{[this] {
            bool shouldTickCloseDispatcher{};

            {
                std::scoped_lock lock{m_mutex};
                if (--m_count == 0 && m_state == State::Closing)
                {
                    m_state = State::Closed;
                    shouldTickCloseDispatcher = true;
                }
            }

            if (!shouldTickCloseDispatcher)
            {
                return;
            }

            auto cancellationSource = TryGetCancellationSource();
            if (cancellationSource)
            {
                m_closeDispatcher.tick(*cancellationSource);
            }
        }});
    }

    std::shared_ptr<arcana::cancellation_source> SafeTimespanGuarantor::TryGetCancellationSource() const
    {
        if (!m_getCancellationSource)
        {
            return {};
        }

        return m_getCancellationSource();
    }
}

#include <Babylon/Graphics/SafeTimespanGuarantor.h>

namespace Babylon::Graphics
{
    SafeTimespanGuarantor::SafeTimespanGuarantor(std::optional<arcana::cancellation_source>& cancellation)
        : m_cancellation{cancellation}
    {
    }

    void SafeTimespanGuarantor::Open()
    {
        std::scoped_lock lock{m_mutex};
        if (m_state == State::Closed)
        {
            m_state = State::Open;
        }
        m_openDispatcher.tick(*m_cancellation);
    }

    void SafeTimespanGuarantor::RequestClose()
    {
        std::scoped_lock lock{m_mutex};
        if (m_state != State::Open)
        {
            throw std::runtime_error{"Safe timespan cannot end if guarantor state is not open"};
        }
        m_state = State::Closed;
        m_closeDispatcher.tick(*m_cancellation);
    }
}

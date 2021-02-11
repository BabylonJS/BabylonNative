#include "SafeTimespanGuarantor.h"

namespace Babylon
{
    SafeTimespanGuarantor::SafeTimespanGuarantor()
        : m_lock{m_mutex}
    {
    }

    void SafeTimespanGuarantor::BeginSafeTimespan()
    {
        m_postCount = 0;
        m_lock.reset();
    }

    void SafeTimespanGuarantor::EndSafeTimespan()
    {
        bool wait{false};
        do
        {
            m_lock.emplace(m_mutex);
            wait = m_postCount > 0;
            --m_postCount;

            if (wait)
            {
                m_lock.reset();
            }
        } while (wait && m_semaphore.wait());
    }

    SafeTimespanGuarantor::SafetyGuarantee SafeTimespanGuarantor::GetSafetyGuarantee()
    {
        std::scoped_lock lock{m_mutex};
        ++m_postCount;
        return m_semaphore.GetPostFinalAction();
    }
}

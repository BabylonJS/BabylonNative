#include "SafeTimespanGuarantor.h"

namespace Babylon
{
    SafeTimespanGuarantor::SafeTimespanGuarantor()
        : m_affinity{ std::this_thread::get_id() }
        , m_lock{ m_mutex }
    {
    }

    void SafeTimespanGuarantor::BeginSafeTimespan()
    {
        if (!m_affinity.check())
        {
            throw std::runtime_error{ "BeginSafeTimespan must be called from the thread on which the SafeTimespanGuarantor was constructed." };
        }

        if (!m_lock.owns_lock())
        {
            throw std::runtime_error{ "EndSafeTimespan must be called before BeginSafeTimespan can be called again." };
        }

        // First unlock the underlying mutex, which allows calls to GetSafetyGuarentee to acquire a SafetyGuarentee.
        m_lock.unlock();

        // Then yield to ensure calls to GetSafetyGuarentee get a chance to lock on the mutex before EndSafeTimeSpan takes the lock (e.g. prevent starvation).
        std::this_thread::yield();
    }

    void SafeTimespanGuarantor::EndSafeTimespan()
    {
        if (!m_affinity.check())
        {
            throw std::runtime_error{ "EndSafeTimespan must be called from the thread on which the SafeTimespanGuarantor was constructed." };
        }

        if (m_lock.owns_lock())
        {
            throw std::runtime_error{ "BeginSafeTimespan must be called before EndSafeTimespan can be called." };
        }

        // First lock on the underlying mutex.
        m_lock.lock();

        // Then wait for the count of outstanding SafeteyGuarentees to reach zero.
        // If the condition is not met, the underlying mutex is unlocked, but we still block on the condition variable, waiting to be signated to recheck the condition.
        // Once the condition is met, then the condition variable unblocks, but the lock on the underlying mutex (re-acquired when checking the condition) is retained.
        m_condition.wait(m_lock, [this]{ return m_count == 0; });
    }

    SafeTimespanGuarantor::SafetyGuarantee SafeTimespanGuarantor::GetSafetyGuarantee()
    {
        // First lock on the underlying mutex and increment the outstanding SafeteyGuarantee count.
        std::lock_guard<std::mutex> guard(m_mutex);
        m_count++;

        // Then return a SafeteyGuarantee that should be held until caller operations are complete.
        return gsl::finally(std::function<void()>{ [this]
        {
            // First lock the underlying mutex and decrement the outstanding SafeteyGuarantee count.
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                m_count--;
            }

            // Then signal the condition variable to recheck the condition.
            m_condition.notify_one();
        }});
    }
}

#pragma once

#include <memory>
#include <shared_mutex>

namespace arcana
{
    class braking_shared_mutex
    {
    public:
        void lock()
        {
            std::unique_lock lock{m_brakingMutex};
            m_sharedMutex.lock();
        }

        void lock_shared()
        {
            std::shared_lock lock{m_brakingMutex};
            m_sharedMutex.lock_shared();
        }

        bool try_lock()
        {
            if (!m_brakingMutex.try_lock())
            {
                return false;
            }

            bool succeeded = m_sharedMutex.try_lock();
            m_sharedMutex.unlock();
            return succeeded;
        }

        bool try_shared_lock()
        {
            if (!m_brakingMutex.try_lock_shared())
            {
                return false;
            }

            bool succeeded = m_sharedMutex.try_lock_shared();
            m_sharedMutex.unlock_shared();
            return succeeded;
        }

        void unlock()
        {
            m_sharedMutex.unlock();
        }

        void unlock_shared()
        {
            m_sharedMutex.unlock_shared();
        }

    private:
        std::shared_mutex m_sharedMutex{};
        std::shared_mutex m_brakingMutex{};
    };
}

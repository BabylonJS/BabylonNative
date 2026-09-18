#pragma once

#include <XR.h>

#include <atomic>
#include <mutex>

namespace xr
{
    // Shared by validation callbacks, but never owns the native session implementation.
    struct ImageTrackingValidationState
    {
        std::vector<ImageTrackingScore> Scores;

        void Deactivate()
        {
            std::scoped_lock lock{m_mutex};
            m_active = false;
        }

        template<typename Callback>
        void Complete(Callback&& applyConfiguration)
        {
            std::scoped_lock lock{m_mutex};
            if (m_active)
            {
                applyConfiguration();
                m_scoresValid.store(true, std::memory_order_release);
            }
        }

        std::vector<ImageTrackingScore>* GetScores()
        {
            return m_scoresValid.load(std::memory_order_acquire) ? &Scores : nullptr;
        }

    private:
        std::mutex m_mutex;
        bool m_active{true};
        std::atomic_bool m_scoresValid{false};
    };
}

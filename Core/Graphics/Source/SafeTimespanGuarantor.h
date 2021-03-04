#pragma once

#include <gsl/gsl>

#include <mutex>
#include <condition_variable>

#include <arcana/threading/affinity.h>

namespace Babylon
{
    class SafeTimespanGuarantor
    {
    public:
        SafeTimespanGuarantor();

        void BeginSafeTimespan();
        void EndSafeTimespan();

        using SafetyGuarantee = gsl::final_action<std::function<void()>>;
        SafetyGuarantee GetSafetyGuarantee();

    private:
        arcana::affinity m_affinity{};
        uint32_t m_count{};
        std::mutex m_mutex{};
        std::unique_lock<std::mutex> m_lock{};
        std::condition_variable m_condition{};
    };
}

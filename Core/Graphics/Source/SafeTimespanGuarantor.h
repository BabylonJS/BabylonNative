#pragma once

#include <bx/semaphore.h>

#include <gsl/gsl>

#include <mutex>
#include <optional>

namespace Babylon
{
    class Semaphore : public bx::Semaphore
    {
    public:
        auto GetPostFinalAction()
        {
            return gsl::finally([this]() {
                post();
            });
        }
    };

    class SafeTimespanGuarantor
    {
    public:
        SafeTimespanGuarantor();

        void BeginSafeTimespan();
        void EndSafeTimespan();

        using SafetyGuarantee = decltype(std::declval<Semaphore>().GetPostFinalAction());
        SafetyGuarantee GetSafetyGuarantee();

    private:
        Semaphore m_semaphore{};
        size_t m_postCount{};
        std::mutex m_mutex{};
        std::optional<std::scoped_lock<std::mutex>> m_lock{};
    };
}

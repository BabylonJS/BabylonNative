#pragma once

#include <bx/semaphore.h>

#include <gsl/gsl>

#include <mutex>
#include <optional>
#include <vector>
#include <deque>

namespace Babylon
{
//    class Semaphore : public bx::Semaphore
//    {
//    public:
//        auto GetPostFinalAction()
//        {
//            return gsl::finally([this]() {
//                post();
//            });
//        }
//    };

    class SafeTimespanGuarantor
    {
    public:
        SafeTimespanGuarantor();

        void BeginSafeTimespan();
        void EndSafeTimespan();

        //using SafetyGuarantee = decltype(std::declval<Semaphore>().GetPostFinalAction());
        //using SafetyGuarantee = std::unique_lock<std::mutex>;
        using SafetyGuarantee = gsl::final_action<std::function<void()>>;
        SafetyGuarantee GetSafetyGuarantee();

    private:
//        Semaphore m_semaphore{};
//        size_t m_postCount{};
//        std::mutex m_mutex{};
//        std::mutex m_mutex{};
//        std::optional<std::scoped_lock<std::mutex>> m_lock{};
//        //std::vector<std::unique_ptr<std::mutex>> m_mutexes{};
//        std::deque<std::mutex> m_mutexes{};
        
//        std::atomic<uint32_t> m_count{};
        uint32_t m_count{};
        std::mutex m_mutex{};
        std::unique_lock<std::mutex> m_lock{};
        std::condition_variable m_condition{};
        bool m_begin{};
    };
}

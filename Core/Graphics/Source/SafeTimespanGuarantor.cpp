#include "SafeTimespanGuarantor.h"

#include <thread>
#include <chrono>

using namespace std::chrono_literals;

namespace Babylon
{
    SafeTimespanGuarantor::SafeTimespanGuarantor()
        : m_lock{ m_mutex }
    {
    }

    void SafeTimespanGuarantor::BeginSafeTimespan()
    {
        if (m_begin)
        {
            throw std::runtime_error{"Already started"};
        }
        m_begin = true;
        m_lock.unlock();
        //std::this_thread::sleep_for(1ms);
        std::this_thread::yield();
        //m_postCount = 0;
//        m_mutexes.clear();
//        m_lock.reset();
    }

    void SafeTimespanGuarantor::EndSafeTimespan()
    {
        if (!m_begin)
        {
            throw std::runtime_error{"Not started"};
        }
//        bool wait{false};
//        do
//        {
//            m_lock.emplace(m_mutex);
//            wait = m_postCount > 0;
//            --m_postCount;
//
//            if (wait)
//            {
//                m_lock.reset();
//            }
//        } while (wait && m_semaphore.wait());

//        m_lock.emplace(m_mutex);
//        for (auto& mutex : m_mutexes)
//        {
//            //std::lock_guard<std::mutex> guard(*(mutex.get()));
//            std::lock_guard<std::mutex> guard(mutex);
//        }
        
        //std::unique_lock<std::mutex> lock(m_mutex);
        m_lock.lock();
        m_condition.wait(m_lock, [this]{ return m_count == 0; });
        m_begin = false;
    }

    SafeTimespanGuarantor::SafetyGuarantee SafeTimespanGuarantor::GetSafetyGuarantee()
    {
//        std::scoped_lock lock{m_mutex};
//        ++m_postCount;
//        return m_semaphore.GetPostFinalAction();
        //std::lock_guard<std::mutex> guard(m_mutex);
        //std::mutex mutex{};
        //m_mutexes.push_back(std::mutex());
        //m_mutexes.emplace_back();
        //m_mutexes.push_back(std::unique_ptr<std::mutex>());
        //m_mutexes.push_back({});
//        m_mutexes.emplace_back();
//        auto& mutex = m_mutexes.back();
//        //return std::unique_lock<std::mutex>{ *(mutex->get()) };
//        return std::unique_lock<std::mutex>(mutex);
        std::lock_guard<std::mutex> guard(m_mutex);
        m_count++;
        return gsl::finally(std::function<void()>{ [this]()
        {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                m_count--;
            }
            m_condition.notify_one();
        }});
    }
}

#pragma once

#include <arcana/threading/dispatcher.h>

namespace Babylon
{
    template<size_t WorkSize = 128>
    class continuation_scheduler
    {
    public:
        continuation_scheduler(arcana::manual_dispatcher<WorkSize>& dispatcher)
            : m_dispatcher{dispatcher}
        {
        }
        continuation_scheduler(const continuation_scheduler&) = delete;
        continuation_scheduler(continuation_scheduler&&) = delete;

        template<typename CallableT>
        void operator()(CallableT&& callable)
        {
            m_dispatcher(callable);
        }

    protected:
        arcana::manual_dispatcher<WorkSize>& m_dispatcher;
    };

    template<size_t WorkSize = 128>
    class continuation_dispatcher
    {
    public:
        continuation_dispatcher()
            : m_dispatcher{}
            , m_scheduler{m_dispatcher}
        {
        }

        auto& scheduler()
        {
            return m_scheduler;
        }

        void tick(const arcana::cancellation& cancellation)
        {
            m_dispatcher.tick(cancellation);
        }
    
    private:
        arcana::manual_dispatcher<WorkSize> m_dispatcher{};
        continuation_scheduler<WorkSize> m_scheduler{};
    };
}

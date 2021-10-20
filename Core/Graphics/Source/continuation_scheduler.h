#pragma once

#include <arcana/threading/dispatcher.h>

namespace Babylon
{
    template<size_t WorkSize = 128>
    class continuation_scheduler
    {
    public:
        template<typename CallableT>
        void operator()(CallableT&& callable)
        {
            m_dispatcher(callable);
        }

    protected:
        arcana::manual_dispatcher<WorkSize> m_dispatcher;
    };

    template<typename T, size_t WorkSize = 128>
    class tickable_continuation_scheduler : public continuation_scheduler<WorkSize>
    {
    private:
        friend T;

        void tick(const arcana::cancellation& cancellation)
        {
            m_dispatcher.tick(cancellation);
        }
    };
}

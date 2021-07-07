#pragma once

#include <GraphicsImpl.h>

#include <arcana/threading/cancellation.h>

namespace Babylon
{
    // This type contains a per-frame value which automatically resets to
    // a provided default at the end of each frame.
    template<typename T>
    class PerFrameValue
    {
    public:
        PerFrameValue(GraphicsImpl& impl, arcana::cancellation_source& cancellation, T defaultValue)
            : m_impl{impl}
            , m_cancellationSource{cancellation}
            , m_defaultValue{defaultValue}
            , m_value{defaultValue}
            , m_isResetScheduled{false}
        {
        }
        
        T Get(bgfx::Encoder&) const
        {
            return m_value;
        }

        void Set(bgfx::Encoder&, bool value)
        {
            m_value = value;
            if (!m_isResetScheduled)
            {
                arcana::make_task(m_impl.AfterRenderScheduler(), m_cancellationSource, [this]() {
                    m_value = m_defaultValue;
                    m_isResetScheduled = false;
                });
                m_isResetScheduled = true;
            }
        }

    private:
        GraphicsImpl& m_impl;
        arcana::cancellation_source& m_cancellationSource;
        const T m_defaultValue{};
        T m_value{};
        bool m_isResetScheduled{};
    };
}

#pragma once

#include <Babylon/Graphics/DeviceContext.h>

#include <arcana/threading/cancellation.h>

#include <memory>

namespace Babylon
{
    // This type contains a per-frame value which automatically resets to
    // a provided default at the end of each frame.
    template<typename T>
    class PerFrameValue
    {
    public:
        PerFrameValue(Graphics::DeviceContext& context, std::shared_ptr<arcana::cancellation_source> cancellation, T defaultValue)
            : m_context{context}
            , m_cancellationSource{std::move(cancellation)}
            , m_defaultValue{defaultValue}
            , m_value{defaultValue}
            , m_isResetScheduled{false}
        {
        }

        T Get() const
        {
            return m_value;
        }

        void Set(bool value)
        {
            m_value = value;
            if (!m_isResetScheduled)
            {
                // The task may outlive this object, so it keeps the cancellation source alive.
                arcana::make_task(m_context.AfterRenderScheduler(), *m_cancellationSource, [this, cancellationSource{m_cancellationSource}]() {
                    m_value = m_defaultValue;
                    m_isResetScheduled = false;
                });
                m_isResetScheduled = true;
            }
        }

    private:
        Graphics::DeviceContext& m_context;
        std::shared_ptr<arcana::cancellation_source> m_cancellationSource;
        const T m_defaultValue{};
        T m_value{};
        bool m_isResetScheduled{};
    };
}

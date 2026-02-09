#pragma once

#include "SafeTimespanGuarantor.h"
#include "continuation_scheduler.h"

#include <napi/env.h>

#include <cstddef>
#include <functional>
#include <vector>

namespace Babylon::Graphics
{
    class Update;
    class DeviceContext;
    class DeviceImpl;

    class UpdateToken final
    {
    public:
        UpdateToken(const UpdateToken&) = delete;
        UpdateToken& operator=(const UpdateToken&) = delete;
        UpdateToken(UpdateToken&&) noexcept = default;

    private:
        friend class Update;

        UpdateToken(DeviceContext&, SafeTimespanGuarantor&);

        [[maybe_unused]] DeviceContext& m_context;
        SafeTimespanGuarantor::SafetyGuarantee m_guarantee;
    };

    class Update
    {
    public:
        continuation_scheduler<>& Scheduler()
        {
            return m_safeTimespanGuarantor.OpenScheduler();
        }

        UpdateToken GetUpdateToken()
        {
            return {m_context, m_safeTimespanGuarantor};
        }

    private:
        friend class DeviceContext;

        Update(SafeTimespanGuarantor& safeTimespanGuarantor, DeviceContext& context)
            : m_safeTimespanGuarantor{safeTimespanGuarantor}
            , m_context{context}
        {
        }

        SafeTimespanGuarantor& m_safeTimespanGuarantor;
        DeviceContext& m_context;
    };

    class DeviceContext
    {
    public:
        explicit DeviceContext(DeviceImpl&);

        static DeviceContext& GetFromJavaScript(Napi::Env);

        static Napi::Value Create(Napi::Env, DeviceImpl&);
        static DeviceContext& GetFromJavaScript(Napi::Value);

        continuation_scheduler<>& BeforeRenderScheduler();
        continuation_scheduler<>& AfterRenderScheduler();

        Update GetUpdate(const char* updateName);

        void RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback);
        void SetRenderResetCallback(std::function<void()> callback);

        float GetHardwareScalingLevel();
        void SetHardwareScalingLevel(float level);

        size_t GetWidth() const;
        size_t GetHeight() const;
        float GetDevicePixelRatio();

        uintptr_t GetDeviceId() const;

    private:
        DeviceImpl& m_graphicsImpl;
    };
}

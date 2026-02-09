#include "DeviceContext.h"

#include "DeviceImpl.h"

#include <napi/pointer.h>

namespace Babylon::Graphics
{
    UpdateToken::UpdateToken(DeviceContext& context, SafeTimespanGuarantor& guarantor)
        : m_context{context}
        , m_guarantee{guarantor.GetSafetyGuarantee()}
    {
    }
}

namespace Babylon::Graphics
{
    DeviceContext& DeviceContext::GetFromJavaScript(Napi::Env env)
    {
        return DeviceImpl::GetFromJavaScript(env).GetContext();
    }

    Napi::Value DeviceContext::Create(Napi::Env env, DeviceImpl& impl)
    {
        auto* context = new DeviceContext(impl);
        return Napi::Pointer<DeviceContext>::Create(env, context, Napi::NapiPointerDeleter(context));
    }

    DeviceContext& DeviceContext::GetFromJavaScript(Napi::Value value)
    {
        return *value.As<Napi::Pointer<DeviceContext>>().Get();
    }

    DeviceContext::DeviceContext(DeviceImpl& graphicsImpl)
        : m_graphicsImpl{graphicsImpl}
    {
    }

    continuation_scheduler<>& DeviceContext::BeforeRenderScheduler()
    {
        return m_graphicsImpl.BeforeRenderScheduler();
    }

    continuation_scheduler<>& DeviceContext::AfterRenderScheduler()
    {
        return m_graphicsImpl.AfterRenderScheduler();
    }

    Update DeviceContext::GetUpdate(const char* updateName)
    {
        return {m_graphicsImpl.GetSafeTimespanGuarantor(updateName), *this};
    }

    void DeviceContext::RequestScreenShot(std::function<void(std::vector<uint8_t>)> callback)
    {
        m_graphicsImpl.RequestScreenShot(std::move(callback));
    }

    void DeviceContext::SetRenderResetCallback(std::function<void()> callback)
    {
        m_graphicsImpl.SetRenderResetCallback(std::move(callback));
    }

    float DeviceContext::GetHardwareScalingLevel()
    {
        return m_graphicsImpl.GetHardwareScalingLevel();
    }

    void DeviceContext::SetHardwareScalingLevel(float level)
    {
        m_graphicsImpl.SetHardwareScalingLevel(level);
    }

    size_t DeviceContext::GetWidth() const
    {
        return m_graphicsImpl.GetWidth();
    }

    size_t DeviceContext::GetHeight() const
    {
        return m_graphicsImpl.GetHeight();
    }

    float DeviceContext::GetDevicePixelRatio()
    {
        return m_graphicsImpl.GetDevicePixelRatio();
    }

    uintptr_t DeviceContext::GetDeviceId() const
    {
        return m_graphicsImpl.GetId();
    }
}

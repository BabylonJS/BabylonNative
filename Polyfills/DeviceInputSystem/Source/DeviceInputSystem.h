#pragma once

#include <Babylon/Polyfills/DeviceInputSystem.h>

#include <napi/napi.h>

namespace Babylon::Plugins::Internal
{
    class DeviceInputSystem final : public Napi::ObjectWrap<DeviceInputSystem>
    {
        static constexpr auto JS_CONSTRUCTOR_NAME = "DeviceInputSystem";

    public:
        static void Initialize(Napi::Env env);

        explicit DeviceInputSystem(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetOnDeviceConnected(const Napi::CallbackInfo& info);
        void SetOnDeviceConnected(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetOnDeviceDisconnected(const Napi::CallbackInfo& info);
        void SetOnDeviceDisconnected(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value PollInput(const Napi::CallbackInfo& info);

        Babylon::Plugins::DeviceInputSystem& m_deviceInputSystem;
        Napi::FunctionReference m_onDeviceConnected;
        Napi::FunctionReference m_onDeviceDisconnected;
        Babylon::Plugins::DeviceInputSystem::DeviceStatusChangedCallbackTicket m_deviceConnectedTicket;
        Babylon::Plugins::DeviceInputSystem::DeviceStatusChangedCallbackTicket m_deviceDisconnectedTicket;
    };
}

#pragma once

#include "NativeInput.h"

namespace Babylon::Plugins
{
    class NativeInput::Impl::DeviceInputSystem final : public Napi::ObjectWrap<DeviceInputSystem>
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

        NativeInput::Impl& m_nativeInput;
        Napi::FunctionReference m_onDeviceConnected;
        Napi::FunctionReference m_onDeviceDisconnected;
        NativeInput::Impl::DeviceStatusChangedCallbackTicket m_deviceConnectedTicket;
        NativeInput::Impl::DeviceStatusChangedCallbackTicket m_deviceDisconnectedTicket;
    };
}

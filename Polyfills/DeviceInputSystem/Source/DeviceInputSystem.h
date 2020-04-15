#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>

#include <unordered_map>

namespace Babylon::Polyfills::Internal
{
    class DeviceInputSystem final : public Napi::ObjectWrap<DeviceInputSystem>
    {
        static constexpr auto JS_CONSTRUCTOR_NAME = "DeviceInputSystem";

    public:
        static void Initialize(Napi::Env env);

        explicit DeviceInputSystem(const Napi::CallbackInfo& info);
        ~DeviceInputSystem();

        void PointerDown(int pointerId, int buttonIndex);
        void PointerUp(int pointerId, int buttonIndex);
        void PointerMove(int pointerId, int x, int y);

    private:
        Napi::Value GetOnDeviceConnected(const Napi::CallbackInfo& info);
        void SetOnDeviceConnected(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value PollInput(const Napi::CallbackInfo& info);

        JsRuntimeScheduler m_runtimeScheduler;
        std::unordered_map<std::string, std::vector<int>> m_inputs;
        Napi::FunctionReference m_onDeviceConnected;
    };
}

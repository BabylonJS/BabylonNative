#pragma once

#include <Babylon/TicketedCollection.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Plugins/NativeInput.h>

#include <optional>
#include <unordered_map>

#include <napi/napi.h>

namespace Babylon::Plugins
{
    class NativeInput::Impl
    {
    public:
        using DeviceStatusChangedCallback = std::function<void(const std::string&)>;
        using DeviceStatusChangedCallbackTicket = TicketedCollection<DeviceStatusChangedCallback>::Ticket;

        Impl(Napi::Env);
        static Impl& GetFromJavaScript(Napi::Env);

        void PointerDown(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y);
        void PointerUp(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y);
        void PointerMove(uint32_t pointerId, uint32_t x, uint32_t y);

        DeviceStatusChangedCallbackTicket AddDeviceConnectedCallback(DeviceStatusChangedCallback&& callback);
        DeviceStatusChangedCallbackTicket AddDeviceDisconnectedCallback(DeviceStatusChangedCallback&& callback);
        std::optional<int32_t> PollInput(const std::string& deviceName, uint32_t inputIndex);

    private:
        std::vector<int32_t>& GetOrCreateInputMap(const std::string& deviceId, const std::vector<uint32_t>& inputIndices);
        void RemoveInputMap(const std::string& deviceId);

    private:
        JsRuntimeScheduler m_runtimeScheduler;
        std::unordered_map<std::string, std::vector<int32_t>> m_inputs{};
        TicketedCollection<DeviceStatusChangedCallback> m_deviceConnectedCallbacks{};
        TicketedCollection<DeviceStatusChangedCallback> m_deviceDisconnectedCallbacks{};

    private:
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

            Babylon::Plugins::NativeInput::Impl& m_nativeInput;
            Napi::FunctionReference m_onDeviceConnected;
            Napi::FunctionReference m_onDeviceDisconnected;
            Babylon::Plugins::NativeInput::Impl::DeviceStatusChangedCallbackTicket m_deviceConnectedTicket;
            Babylon::Plugins::NativeInput::Impl::DeviceStatusChangedCallbackTicket m_deviceDisconnectedTicket;
        };
    };
}

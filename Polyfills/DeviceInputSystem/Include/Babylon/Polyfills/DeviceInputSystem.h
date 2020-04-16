#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/TicketedCollection.h>

#include <optional>
#include <unordered_map>

#include <napi/env.h>

namespace Babylon::Plugins
{
    class DeviceInputSystem
    {
    public:
        using DeviceStatusChangedCallback = std::function<void(const std::string&)>;
        using DeviceStatusChangedCallbackTicket = TicketedCollection<DeviceStatusChangedCallback>::Ticket;

        // TODO: Ideally instances of these should be scoped to individual views within an env, but we don't yet support multi-view.
        static DeviceInputSystem& CreateForJavaScript(Napi::Env);
        static DeviceInputSystem& GetFromJavaScript(Napi::Env);

        void PointerDown(uint32_t pointerId, uint32_t buttonIndex);
        void PointerUp(uint32_t pointerId, uint32_t buttonIndex);
        void PointerMove(uint32_t pointerId, uint32_t x, uint32_t y);

        DeviceStatusChangedCallbackTicket AddDeviceConnectedCallback(DeviceStatusChangedCallback&& callback);
        DeviceStatusChangedCallbackTicket AddDeviceDisconnectedCallback(DeviceStatusChangedCallback&& callback);
        std::optional<int32_t> PollInput(const std::string& deviceName, uint32_t inputIndex);

    protected:
        DeviceInputSystem(const DeviceInputSystem&) = delete;
        DeviceInputSystem(DeviceInputSystem&&) = delete;

    private:
        DeviceInputSystem(Napi::Env);
        std::vector<int32_t>& GetOrCreateInputMap(const std::string& deviceId, uint32_t inputIndex);
        void RemoveInputMap(const std::string& deviceId);

    private:
        JsRuntimeScheduler m_runtimeScheduler;
        std::unordered_map<std::string, std::vector<int32_t>> m_inputs{};
        TicketedCollection<DeviceStatusChangedCallback> m_deviceConnectedCallbacks{};
        TicketedCollection<DeviceStatusChangedCallback> m_deviceDisconnectedCallbacks{};
    };
}

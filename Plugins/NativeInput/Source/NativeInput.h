#pragma once

#include <Babylon/TicketedCollection.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Plugins/NativeInput.h>

#include <optional>
#include <unordered_map>

namespace Babylon::Plugins
{
    class NativeInput::Impl
    {
    public:
        using DeviceStatusChangedCallback = std::function<void(const std::string&)>;
        using DeviceStatusChangedCallbackTicket = TicketedCollection<DeviceStatusChangedCallback>::Ticket;

        Impl(Napi::Env);

        void PointerDown(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y);
        void PointerUp(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y);
        void PointerMove(uint32_t pointerId, uint32_t x, uint32_t y);

        DeviceStatusChangedCallbackTicket AddDeviceConnectedCallback(DeviceStatusChangedCallback&& callback);
        DeviceStatusChangedCallbackTicket AddDeviceDisconnectedCallback(DeviceStatusChangedCallback&& callback);
        std::optional<int32_t> PollInput(const std::string& deviceName, uint32_t inputIndex);

    private:
        std::vector<int32_t>& GetOrCreateInputMap(const std::string& deviceId, const std::vector<uint32_t>& inputIndices);
        void RemoveInputMap(const std::string& deviceId);

        JsRuntimeScheduler m_runtimeScheduler;
        std::unordered_map<std::string, std::vector<int32_t>> m_inputs{};
        TicketedCollection<DeviceStatusChangedCallback> m_deviceConnectedCallbacks{};
        TicketedCollection<DeviceStatusChangedCallback> m_deviceDisconnectedCallbacks{};

        class DeviceInputSystem;
    };
}

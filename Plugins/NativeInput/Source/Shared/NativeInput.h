#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Plugins/NativeInput.h>
#include <arcana/containers/weak_table.h>
#include <arcana/threading/dispatcher.h>

#include <optional>
#include <unordered_map>

namespace Babylon::Plugins
{
    class NativeInput::Impl final
    {
    public:
        enum class DeviceType
        {
            Generic = 0,
            Keyboard = 1,
            Mouse = 2,
            Touch = 3,
            DualShock = 4,
            Xbox = 5,
            Switch = 6,
        };

        using DeviceStatusChangedCallback = std::function<void(DeviceType deviceType, int32_t deviceSlot)>;
        using DeviceStatusChangedCallbackTicket = arcana::weak_table<DeviceStatusChangedCallback>::ticket;

        using InputStateChangedCallback = std::function<void(DeviceType deviceType, int32_t deviceSlot, uint32_t inputIndex, std::optional<int32_t> currentState)>;
        using InputStateChangedCallbackTicket = arcana::weak_table<InputStateChangedCallback>::ticket;

        Impl(Napi::Env);

        bool HasMouse();

        void MouseDown(uint32_t buttonIndex, int32_t x, int32_t y);
        void MouseUp(uint32_t buttonIndex, int32_t x, int32_t y);
        void MouseMove(int32_t x, int32_t y);
        void TouchDown(uint32_t pointerId, int32_t x, int32_t y);
        void TouchUp(uint32_t pointerId, int32_t x, int32_t y);
        void TouchMove(uint32_t pointerId, int32_t x, int32_t y);

        DeviceStatusChangedCallbackTicket AddDeviceConnectedCallback(DeviceStatusChangedCallback&& callback);
        DeviceStatusChangedCallbackTicket AddDeviceDisconnectedCallback(DeviceStatusChangedCallback&& callback);
        InputStateChangedCallbackTicket AddInputChangedCallback(InputStateChangedCallback&& callback);
        int32_t PollInput(DeviceType deviceType, int32_t deviceSlot, uint32_t inputIndex);
        bool IsDeviceAvailable(DeviceType deviceType);

    private:
        using InputMapKey = std::pair<DeviceType, int32_t>;

        struct InputMapKeyHash
        {
            size_t operator()(const InputMapKey& inputMapKey) const
            {
                return std::hash<uint64_t>{}((static_cast<uint64_t>(inputMapKey.first) << (sizeof(uint32_t) * 8)) + static_cast<uint32_t>(inputMapKey.second));
            }
        };

        void PointerDown(uint32_t pointerId, uint32_t buttonIndex, int32_t x, int32_t y, DeviceType deviceType);
        void PointerUp(uint32_t pointerId, uint32_t buttonIndex, int32_t x, int32_t y, DeviceType deviceType);
        void PointerMove(uint32_t pointerId, int32_t x, int32_t y, DeviceType deviceType);
        std::vector<int32_t>& GetOrCreateInputMap(DeviceType deviceType, int32_t deviceSlot, const std::vector<uint32_t>& inputIndices);
        void RemoveInputMap(DeviceType deviceType, int32_t deviceSlot);
        void SetInputState(DeviceType deviceType, int32_t deviceSlot, uint32_t inputIndex, int32_t inputState, std::vector<int32_t>& deviceInputs, bool raiseEvents);

        JsRuntimeScheduler m_runtimeScheduler;
        std::unordered_map<InputMapKey, std::vector<int32_t>, InputMapKeyHash> m_inputs{};
        arcana::weak_table<DeviceStatusChangedCallback> m_deviceConnectedCallbacks{};
        arcana::weak_table<DeviceStatusChangedCallback> m_deviceDisconnectedCallbacks{};
        arcana::weak_table<InputStateChangedCallback> m_inputChangedCallbacks{};

        arcana::manual_dispatcher<64> m_eventDispatcher{};

        bool m_getDelta{false};

        class DeviceInputSystem;
    };
}

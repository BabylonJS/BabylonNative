#include "NativeInput.h"
#include "DeviceInputSystem.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeInput.h>

#include <algorithm>
#include <sstream>
#include <limits>

namespace Babylon::Plugins
{
    namespace
    {
        constexpr auto JS_NATIVE_INPUT_NAME = "_nativeInput";

        constexpr uint32_t POINTER_X_INPUT_INDEX{0};
        constexpr uint32_t POINTER_Y_INPUT_INDEX{1};
        constexpr uint32_t POINTER_BUTTON_BASE_INDEX{2};
        constexpr uint32_t POINTER_BUTTON_LEFT_INDEX{2};
        constexpr uint32_t POINTER_DELTA_HORIZONTAL_INDEX{10};
        constexpr uint32_t POINTER_DELTA_VERTICAL_INDEX{11};
        constexpr uint32_t POINTER_MOVE_INDEX{12};
        constexpr uint32_t MOUSE_POINTER_ID{0};

        constexpr uint32_t GetPointerButtonInputIndex(uint32_t buttonIndex)
        {
            return POINTER_BUTTON_BASE_INDEX + buttonIndex;
        }
    }

    NativeInput::NativeInput(Napi::Env env)
        : m_impl{ std::make_unique<Impl>(env) }
    {
        Napi::Value nativeInput = Napi::External<NativeInput>::New(env, this, [](Napi::Env, NativeInput* nativeInput) { delete nativeInput; });
        env.Global().Set(JS_NATIVE_INPUT_NAME, nativeInput);
    }

    NativeInput& NativeInput::CreateForJavaScript(Napi::Env env)
    {
        auto* nativeInput = new NativeInput(env);
        return *nativeInput;
    }

    NativeInput& NativeInput::GetFromJavaScript(Napi::Env env)
    {
        return *env.Global().Get(JS_NATIVE_INPUT_NAME).As<Napi::External<NativeInput>>().Data();
    }

    void NativeInput::MouseDown(uint32_t buttonIndex, int32_t x, int32_t y)
    {
        m_impl->MouseDown(buttonIndex, x, y);
    }

    void NativeInput::MouseUp(uint32_t buttonIndex, int32_t x, int32_t y)
    {
        m_impl->MouseUp(buttonIndex, x, y);
    }

    void NativeInput::MouseMove(int32_t x, int32_t y)
    {
        m_impl->MouseMove(x, y);
    }

    void NativeInput::TouchDown(uint32_t pointerId, int32_t x, int32_t y)
    {
        m_impl->TouchDown(pointerId, x, y);
    }

    void NativeInput::TouchUp(uint32_t pointerId, int32_t x, int32_t y)
    {
        m_impl->TouchUp(pointerId, x, y);
    }

    void NativeInput::TouchMove(uint32_t pointerId, int32_t x, int32_t y)
    {
        m_impl->TouchMove(pointerId, x, y);
    }

    NativeInput::Impl::Impl(Napi::Env env)
        : m_runtimeScheduler{JsRuntime::GetFromJavaScript(env)}
    {
        NativeInput::Impl::DeviceInputSystem::Initialize(env);

        if (HasMouse())
        {
            // Create a mouse input map on initialization when available to match web behavior
            GetOrCreateInputMap(DeviceType::Mouse, MOUSE_POINTER_ID,
                {
                    POINTER_X_INPUT_INDEX,
                    POINTER_Y_INPUT_INDEX,
                    POINTER_BUTTON_LEFT_INDEX,
                    POINTER_DELTA_HORIZONTAL_INDEX,
                    POINTER_DELTA_VERTICAL_INDEX,
                    POINTER_MOVE_INDEX
                });
        }
    }

    void NativeInput::Impl::MouseDown(uint32_t buttonIndex, int32_t x, int32_t y)
    {
        PointerDown(MOUSE_POINTER_ID, buttonIndex, x, y, DeviceType::Mouse);
    }

    void NativeInput::Impl::MouseUp(uint32_t buttonIndex, int32_t x, int32_t y)
    {
        PointerUp(MOUSE_POINTER_ID, buttonIndex, x, y, DeviceType::Mouse);
    }

    void NativeInput::Impl::MouseMove(int32_t x, int32_t y)
    {
        PointerMove(MOUSE_POINTER_ID, x, y, DeviceType::Mouse);
    }

    void NativeInput::Impl::TouchDown(uint32_t pointerId, int32_t x, int32_t y)
    {
        PointerDown(pointerId, TOUCH_BUTTON_ID, x, y, DeviceType::Touch);
    }

    void NativeInput::Impl::TouchUp(uint32_t pointerId, int32_t x, int32_t y)
    {
        PointerUp(pointerId, TOUCH_BUTTON_ID, x, y, DeviceType::Touch);
    }

    void NativeInput::Impl::TouchMove(uint32_t pointerId, int32_t x, int32_t y)
    {
        PointerMove(pointerId, x, y, DeviceType::Touch);
    }

    void NativeInput::Impl::PointerDown(uint32_t pointerId, uint32_t buttonIndex, int32_t x, int32_t y, DeviceType deviceType)
    {
        m_runtimeScheduler([pointerId, buttonIndex, x, y, deviceType, this]() {
            const uint32_t inputIndex{ GetPointerButtonInputIndex(buttonIndex) };
            std::vector<int32_t>& deviceInputs{ GetOrCreateInputMap(deviceType, pointerId, {
                inputIndex,
                POINTER_X_INPUT_INDEX,
                POINTER_Y_INPUT_INDEX,
                POINTER_DELTA_HORIZONTAL_INDEX,
                POINTER_DELTA_VERTICAL_INDEX
            })};

            // Record the x/y, but don't raise associated events (this matches the behavior in the browser).
            SetInputState(deviceType, pointerId, POINTER_DELTA_HORIZONTAL_INDEX, 0, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_DELTA_VERTICAL_INDEX, 0, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_X_INPUT_INDEX, x, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_Y_INPUT_INDEX, y, deviceInputs, false);
            SetInputState(deviceType, pointerId, inputIndex, 1, deviceInputs, true);

            m_eventDispatcher.tick(arcana::cancellation::none());
        });
    }

    void NativeInput::Impl::PointerUp(uint32_t pointerId, uint32_t buttonIndex, int32_t x, int32_t y, DeviceType deviceType)
    {
        m_runtimeScheduler([pointerId, buttonIndex, x, y, deviceType, this]() {
            const uint32_t inputIndex{ GetPointerButtonInputIndex(buttonIndex) };
            std::vector<int32_t>& deviceInputs{ GetOrCreateInputMap(deviceType, pointerId, {
                inputIndex,
                POINTER_X_INPUT_INDEX,
                POINTER_Y_INPUT_INDEX,
                POINTER_DELTA_HORIZONTAL_INDEX,
                POINTER_DELTA_VERTICAL_INDEX
            })};

            // Record the x/y, but don't raise associated events (this matches the behavior in the browser).
            SetInputState(deviceType, pointerId, POINTER_DELTA_HORIZONTAL_INDEX, 0, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_DELTA_VERTICAL_INDEX, 0, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_X_INPUT_INDEX, x, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_Y_INPUT_INDEX, y, deviceInputs, false);
            SetInputState(deviceType, pointerId, inputIndex, 0, deviceInputs, true);

            m_eventDispatcher.tick(arcana::cancellation::none());

            // Cleanup mouse device in destructor
            if (deviceType == DeviceType::Mouse)
            {
                return;
            }

            // If all "buttons" are up, then remove the device (e.g. device "disconnected").
            for (size_t index = 0; index < deviceInputs.size(); index++)
            {
                if (index != POINTER_X_INPUT_INDEX &&
                    index != POINTER_Y_INPUT_INDEX &&
                    index != POINTER_DELTA_HORIZONTAL_INDEX &&
                    index != POINTER_DELTA_VERTICAL_INDEX &&
                    index != POINTER_MOVE_INDEX &&
                    deviceInputs[index] > 0)
                {
                    return;
                }
            }

            RemoveInputMap(deviceType, pointerId);
            m_eventDispatcher.tick(arcana::cancellation::none());
        });
    }

    void NativeInput::Impl::PointerMove(uint32_t pointerId, int32_t x, int32_t y, DeviceType deviceType)
    {
        m_runtimeScheduler([pointerId, x, y, deviceType, this]() {
            std::vector<int32_t>& deviceInputs{GetOrCreateInputMap(deviceType, pointerId, {
                POINTER_X_INPUT_INDEX,
                POINTER_Y_INPUT_INDEX,
                POINTER_DELTA_HORIZONTAL_INDEX,
                POINTER_DELTA_VERTICAL_INDEX,
                POINTER_MOVE_INDEX
            })};
            int32_t deltaX = 0;
            int32_t deltaY = 0;

            if (m_getDelta)
            {
                deltaX = x - deviceInputs[POINTER_X_INPUT_INDEX];
                deltaY = y - deviceInputs[POINTER_Y_INPUT_INDEX];
            }
            else
            {
                m_getDelta = true;
            }

            // Populate movement values
            SetInputState(deviceType, pointerId, POINTER_DELTA_HORIZONTAL_INDEX, deltaX, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_DELTA_VERTICAL_INDEX, deltaY, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_X_INPUT_INDEX, x, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_Y_INPUT_INDEX, y, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_MOVE_INDEX, 1, deviceInputs, true);
            m_eventDispatcher.tick(arcana::cancellation::none());

            // Zero out deltas
            SetInputState(deviceType, pointerId, POINTER_DELTA_HORIZONTAL_INDEX, 0, deviceInputs, false);
            SetInputState(deviceType, pointerId, POINTER_DELTA_VERTICAL_INDEX, 0, deviceInputs, false);
            m_eventDispatcher.tick(arcana::cancellation::none());

        });
    }

    NativeInput::Impl::DeviceStatusChangedCallbackTicket NativeInput::Impl::AddDeviceConnectedCallback(NativeInput::Impl::DeviceStatusChangedCallback&& callback)
    {
        // Firing this callback on registration to mimic web behavior
        for (auto [key, state] : m_inputs)
        {
            callback(key.first, key.second);
        }

        return  m_deviceConnectedCallbacks.insert(std::move(callback));
    }

    NativeInput::Impl::DeviceStatusChangedCallbackTicket NativeInput::Impl::AddDeviceDisconnectedCallback(NativeInput::Impl::DeviceStatusChangedCallback&& callback)
    {
        return m_deviceDisconnectedCallbacks.insert(std::move(callback));
    }

    NativeInput::Impl::InputStateChangedCallbackTicket NativeInput::Impl::AddInputChangedCallback(NativeInput::Impl::InputStateChangedCallback &&callback)
    {
        return m_inputChangedCallbacks.insert(std::move(callback));
    }

    int32_t NativeInput::Impl::PollInput(DeviceType deviceType, int32_t deviceSlot, uint32_t inputIndex)
    {
        auto it = m_inputs.find({deviceType, deviceSlot});
        if (it == m_inputs.end())
        {
            std::ostringstream message;
            message << "Unable to find device of type " << static_cast<uint32_t>(deviceType) << " with slot " << deviceSlot;
            throw std::runtime_error{ message.str() };
        }

        const auto& device = it->second;
        if (inputIndex >= device.size())
        {
            std::ostringstream message;
            message << "Unable to find " << inputIndex << " on device of type " << static_cast<uint32_t>(deviceType) << " with slot " << deviceSlot;
            throw std::runtime_error{ message.str() };
        }

        return device.at(inputIndex);
    }

    bool NativeInput::Impl::IsDeviceAvailable(DeviceType deviceType)
    {
        return (deviceType == DeviceType::Mouse || deviceType == DeviceType::Touch);
    }

    std::vector<int32_t>& NativeInput::Impl::GetOrCreateInputMap(DeviceType deviceType, int32_t deviceSlot, const std::vector<uint32_t>& inputIndices)
    {
        uint32_t inputIndex = *std::max_element(inputIndices.begin(), inputIndices.end());

        auto previousSize = m_inputs.size();
        std::vector<int32_t>& deviceInputs{m_inputs[{deviceType, deviceSlot}]};
        auto newSize = m_inputs.size();

        if (newSize != previousSize)
        {
            m_eventDispatcher.queue([this, deviceType, deviceSlot]() {
                m_deviceConnectedCallbacks.apply_to_all([deviceType, deviceSlot](auto& callback) {
                    callback(deviceType, deviceSlot);
                });
            });
        }

        deviceInputs.resize(std::max(deviceInputs.size(), static_cast<size_t>(inputIndex + 1)));

        return deviceInputs;
    }

    void NativeInput::Impl::RemoveInputMap(DeviceType deviceType, int32_t deviceSlot)
    {
        if (m_inputs.erase({deviceType, deviceSlot}))
        {
            m_eventDispatcher.queue([this, deviceType, deviceSlot]() {
                m_deviceDisconnectedCallbacks.apply_to_all([deviceType, deviceSlot](auto& callback){
                    callback(deviceType, deviceSlot);
                });
            });
        }
    }

    void NativeInput::Impl::SetInputState(DeviceType deviceType, int32_t deviceSlot, uint32_t inputIndex, int32_t inputState, std::vector<int32_t>& deviceInputs, bool raiseEvents)
    {
        std::optional<int32_t> previousState = deviceInputs[inputIndex];
        if (previousState != inputState || inputIndex == POINTER_MOVE_INDEX)
        {
            deviceInputs[inputIndex] = inputState;
            if (raiseEvents)
            {
                m_eventDispatcher.queue([this, deviceType, deviceSlot, inputIndex, inputState]() {
                    m_inputChangedCallbacks.apply_to_all([deviceType, deviceSlot, inputIndex, inputState](auto& callback) {
                        callback(deviceType, deviceSlot, inputIndex, inputState);
                    });
                });
            }
        }
    }
}

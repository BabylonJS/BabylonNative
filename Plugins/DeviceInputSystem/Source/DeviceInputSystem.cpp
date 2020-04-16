#include "DeviceInputSystem.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/DeviceInputSystem.h>

#include <sstream>

namespace Babylon::Plugins
{
    namespace
    {
        constexpr auto JS_DEVICE_INPUT_SYSTEM_NAME = "_deviceInputSystem";

        const std::string POINTER_BASE_DEVICE_ID{"Pointer"};

        constexpr uint32_t POINTER_X_INPUT_INDEX{0};
        constexpr uint32_t POINTER_Y_INPUT_INDEX{1};
        constexpr uint32_t POINTER_BUTTON_BASE_INDEX{2};

        std::string GetPointerDeviceId(uint32_t pointerId)
        {
            std::ostringstream deviceId;
            deviceId << POINTER_BASE_DEVICE_ID << "-" << pointerId;
            return deviceId.str();
        }

        constexpr uint32_t GetPointerButtonInputIndex(uint32_t buttonIndex)
        {
            return POINTER_BUTTON_BASE_INDEX + buttonIndex;
        }
    }

    DeviceInputSystem::DeviceInputSystem(Napi::Env env)
    {
        Napi::Value deviceInputSystem = Napi::External<DeviceInputSystem>::New(env, this, [](Napi::Env, DeviceInputSystem* deviceInputSystem) { delete deviceInputSystem; });
        env.Global().Set(JS_DEVICE_INPUT_SYSTEM_NAME, deviceInputSystem);
    }

    DeviceInputSystem& DeviceInputSystem::CreateForJavaScript(Napi::Env env)
    {
        Babylon::Plugins::Internal::DeviceInputSystem::Initialize(env);
        auto* deviceInputSystem = new DeviceInputSystem(env);
        return *deviceInputSystem;
    }

    DeviceInputSystem& DeviceInputSystem::GetFromJavaScript(Napi::Env env)
    {
        return *env.Global().Get(JS_DEVICE_INPUT_SYSTEM_NAME).As<Napi::External<DeviceInputSystem>>().Data();
    }

    void DeviceInputSystem::PointerDown(uint32_t pointerId, uint32_t buttonIndex)
    {
        const std::string deviceId{GetPointerDeviceId(pointerId)};
        const uint32_t inputIndex{GetPointerButtonInputIndex(buttonIndex)};
        std::vector<int>& deviceInputs{GetOrCreateInputMap(deviceId, inputIndex)};
        deviceInputs[buttonIndex] = 1;
    }

    void DeviceInputSystem::PointerUp(uint32_t pointerId, uint32_t buttonIndex)
    {
        const std::string deviceId{GetPointerDeviceId(pointerId)};
        RemoveInputMap(deviceId);
    }

    void DeviceInputSystem::PointerMove(uint32_t pointerId, uint32_t x, uint32_t y)
    {
        const std::string deviceId{GetPointerDeviceId(pointerId)};
        std::vector<int>& deviceInputs{GetOrCreateInputMap(deviceId, std::max(POINTER_X_INPUT_INDEX, POINTER_Y_INPUT_INDEX))};
        deviceInputs[POINTER_X_INPUT_INDEX] = x;
        deviceInputs[POINTER_Y_INPUT_INDEX] = y;
    }

    DeviceInputSystem::DeviceStatusChangedCallbackTicket DeviceInputSystem::AddDeviceConnectedCallback(DeviceInputSystem::DeviceStatusChangedCallback&& callback)
    {
        return m_deviceConnectedCallbacks.Insert(std::move(callback));
    }

    DeviceInputSystem::DeviceStatusChangedCallbackTicket DeviceInputSystem::AddDeviceDisconnectedCallback(DeviceInputSystem::DeviceStatusChangedCallback&& callback)
    {
        return m_deviceDisconnectedCallbacks.Insert(std::move(callback));
    }

    std::optional<int32_t> DeviceInputSystem::PollInput(const std::string& deviceName, uint32_t inputIndex)
    {
        auto it = m_inputs.find(deviceName);
        if (it == m_inputs.end())
        {
            std::ostringstream message;
            message << "Unable to find device " + deviceName;
            throw std::runtime_error{ message.str() };
        }

        const auto& device = it->second;
        if (inputIndex >= device.size())
        {
            std::ostringstream message;
            message << "Unable to find " << inputIndex << " on device " << deviceName;
            throw std::runtime_error{ message.str() };
        }

        int32_t inputValue = device.at(inputIndex);
        if (inputValue >= 0)
        {
            return inputValue;
        }
        else
        {
            return {};
        }
    }

    std::vector<int32_t>& DeviceInputSystem::GetOrCreateInputMap(const std::string& deviceId, uint32_t inputIndex)
    {
        auto previousSize = m_inputs.size();
        std::vector<int32_t>& deviceInputs{m_inputs[deviceId]};

        if (m_inputs.size() != previousSize)
        {
            m_deviceConnectedCallbacks.ApplyToAll([deviceId](auto& callback) {
                callback(deviceId);
            });
        }

        deviceInputs.resize(std::max(deviceInputs.size(), static_cast<size_t>(inputIndex)));

        return deviceInputs;
    }

    void DeviceInputSystem::RemoveInputMap(const std::string& deviceId)
    {
        if (m_inputs.erase(deviceId))
        {
            m_deviceDisconnectedCallbacks.ApplyToAll([deviceId](auto& callback){
               callback(deviceId);
            });
        }
    }
}

namespace Babylon::Plugins::Internal
{
    void DeviceInputSystem::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func
        {
            DefineClass(
                env,
                JS_CONSTRUCTOR_NAME,
                {
                    InstanceAccessor("onDeviceConnected", &DeviceInputSystem::GetOnDeviceConnected, &DeviceInputSystem::SetOnDeviceConnected),
                    InstanceAccessor("onDeviceDisconnected", &DeviceInputSystem::GetOnDeviceDisconnected, &DeviceInputSystem::SetOnDeviceDisconnected),
                    InstanceMethod("pollInput", &DeviceInputSystem::PollInput),
                })
        };

        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    DeviceInputSystem::DeviceInputSystem(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DeviceInputSystem>{info}
        , m_deviceInputSystem{Babylon::Plugins::DeviceInputSystem::GetFromJavaScript(info.Env())}
        , m_deviceConnectedTicket{m_deviceInputSystem.AddDeviceConnectedCallback([this](const std::string& deviceId) {
            if (m_onDeviceConnected.Value().IsFunction())
            {
                Napi::Value napiDeviceId = Napi::String::New(Env(), deviceId);
                m_onDeviceConnected.Call({napiDeviceId});
            }
        })}
        , m_deviceDisconnectedTicket{m_deviceInputSystem.AddDeviceDisconnectedCallback([this](const std::string& deviceId) {
            if (m_onDeviceDisconnected.Value().IsFunction())
            {
                Napi::Value napiDeviceId = Napi::String::New(Env(), deviceId);
                m_onDeviceDisconnected.Call({napiDeviceId});
            }
        })}
    {
    }

    Napi::Value DeviceInputSystem::GetOnDeviceConnected(const Napi::CallbackInfo&)
    {
        return m_onDeviceConnected.Value();
    }

    void DeviceInputSystem::SetOnDeviceConnected(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_onDeviceConnected = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value DeviceInputSystem::GetOnDeviceDisconnected(const Napi::CallbackInfo&)
    {
        return m_onDeviceDisconnected.Value();
    }

    void DeviceInputSystem::SetOnDeviceDisconnected(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_onDeviceDisconnected = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value DeviceInputSystem::PollInput(const Napi::CallbackInfo& info)
    {
        std::string deviceName = info[0].As<Napi::String>().Utf8Value();
        uint32_t inputIndex = info[1].As<Napi::Number>().Uint32Value();
        std::optional<int32_t> inputValue = m_deviceInputSystem.PollInput(deviceName, inputIndex);
        return inputValue ? Napi::Value::From(Env(), *inputValue) : Env().Null();
    }
}
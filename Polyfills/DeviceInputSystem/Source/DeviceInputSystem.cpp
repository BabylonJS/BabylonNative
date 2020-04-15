#include "DeviceInputSystem.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/DeviceInputSystem.h>

#include <sstream>
#include <unordered_set>

namespace
{
    // TODO: We should actually map views to DeviceInputSystem instances.
    std::unordered_set<Babylon::Polyfills::Internal::DeviceInputSystem*> g_instances;
}

namespace Babylon::Polyfills::Internal
{
    const std::string POINTER_BASE_DEVICE_ID{"Pointer"};

    constexpr int POINTER_X_INPUT_INDEX{0};
    constexpr int POINTER_Y_INPUT_INDEX{1};
    constexpr int POINTER_BUTTON_BASE_INDEX{2};

    std::string GetPointerDeviceId(int pointerId)
    {
        std::ostringstream deviceId;
        deviceId << POINTER_BASE_DEVICE_ID << "-" << pointerId;
        return deviceId.str();
    }

    constexpr int GetPointerButtonInputIndex(int buttonIndex)
    {
        return POINTER_BUTTON_BASE_INDEX + buttonIndex;
    }

    void DeviceInputSystem::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func
        {
            DefineClass(
                env,
                JS_CONSTRUCTOR_NAME,
                {
                        InstanceAccessor("onDeviceConnected",
                                         &DeviceInputSystem::GetOnDeviceConnected,
                                         &DeviceInputSystem::SetOnDeviceConnected),
                        InstanceMethod("pollInput", &DeviceInputSystem::PollInput),
                })
        };

        env.Global().Set(JS_CONSTRUCTOR_NAME, func);
    }

    DeviceInputSystem::DeviceInputSystem(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DeviceInputSystem>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
    {
        g_instances.insert(this);
    }

    DeviceInputSystem::~DeviceInputSystem()
    {
        g_instances.erase(this);
    }

    void DeviceInputSystem::PointerDown(int pointerId, int buttonIndex)
    {
        m_runtimeScheduler([pointerId, buttonIndex, this]() {
            const std::string deviceId{GetPointerDeviceId(pointerId)};
            const int inputIndex{GetPointerButtonInputIndex(buttonIndex)};
            std::vector<int>& deviceInputs{m_inputs[deviceId]};

            // TODO: Call m_onDeviceConnected if this causes deviceId to be added to the map
            if (m_onDeviceConnected.Value().IsFunction())
            {
                Napi::Value napiDeviceId = Napi::String::New(Env(), deviceId);
                m_onDeviceConnected.Call({napiDeviceId});
            }

            deviceInputs[buttonIndex] = 1;
        });
    }

    void DeviceInputSystem::PointerUp(int pointerId, int buttonIndex)
    {
        m_runtimeScheduler([pointerId, buttonIndex, this]() {
            const std::string deviceId{GetPointerDeviceId(pointerId)};
            const int inputIndex{GetPointerButtonInputIndex(buttonIndex)};
            std::vector<int>& deviceInputs{m_inputs[deviceId]};

            // TODO: Call m_onDeviceConnected if this causes deviceId to be removed from the map

            deviceInputs[buttonIndex] = 0;
        });
    }

    void DeviceInputSystem::PointerMove(int pointerId, int x, int y)
    {
        m_runtimeScheduler([pointerId, x, y, this]() {
            const std::string deviceId{GetPointerDeviceId(pointerId)};
            std::vector<int>& deviceInputs{m_inputs[deviceId]};
            deviceInputs[POINTER_X_INPUT_INDEX] = x;
            deviceInputs[POINTER_Y_INPUT_INDEX] = y;
        });
    }

    Napi::Value DeviceInputSystem::GetOnDeviceConnected(const Napi::CallbackInfo&)
    {
        return m_onDeviceConnected.Value();
    }

    void DeviceInputSystem::SetOnDeviceConnected(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_onDeviceConnected = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value DeviceInputSystem::PollInput(const Napi::CallbackInfo& info)
    {
        std::string deviceName = info[0].As<Napi::String>().Utf8Value();
        uint32_t inputIndex = info[1].As<Napi::Number>().Uint32Value();

        auto it = m_inputs.find(deviceName);
        if (it == m_inputs.end())
        {
            std::ostringstream message;
            message << "Unable to find device " + deviceName;
            throw Napi::Error::New(Env(), message.str());
        }

        const auto& device = it->second;
        if (inputIndex >= device.size())
        {
            std::ostringstream message;
            message << "Unable to find " << inputIndex << " on device " << deviceName;
            throw Napi::Error::New(Env(), message.str());
        }

        int32_t inputValue = device.at(inputIndex);
        return inputValue >= 0 ? Napi::Value::From(Env(), inputValue) : Napi::Value();
    }
}

namespace Babylon::Polyfills::DeviceInputSystem
{
    void Initialize(Napi::Env env)
    {
        Internal::DeviceInputSystem::Initialize(env);
    }

    void PointerDown()
    {
        for (const auto& deviceInputSystem: g_instances)
        {
            //deviceInputSystem->PointerDown();
        }
    }

    void PointerUp()
    {
        for (const auto& deviceInputSystem: g_instances)
        {
            //deviceInputSystem->PointerUp();
        }
    }

    void PointerMove(int x, int y)
    {
        for (const auto& deviceInputSystem: g_instances)
        {
            //deviceInputSystem->PointerMove(x, y);
        }
    }
}

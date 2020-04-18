#include "NativeInput.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Plugins/NativeInput.h>

#include <sstream>

namespace Babylon::Plugins
{
    namespace
    {
        constexpr auto JS_NATIVE_INPUT_NAME = "_nativeInput";

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

    NativeInput::Impl::Impl(Napi::Env env)
        : m_runtimeScheduler{JsRuntime::GetFromJavaScript(env)}
    {
        Babylon::Plugins::NativeInput::Impl::DeviceInputSystem::Initialize(env);
        Napi::Value nativeInput = Napi::External<NativeInput::Impl>::New(env, this, [](Napi::Env, NativeInput::Impl* nativeInput) { delete nativeInput; }); // TODO: This needs to be in NativeInput otherwise we leak NativeInput
        env.Global().Set(JS_NATIVE_INPUT_NAME, nativeInput);
    }

    NativeInput::NativeInput(Napi::Env env)
        : m_impl{ std::make_unique<Impl>(env) }
    {
    }

    NativeInput& NativeInput::CreateForJavaScript(Napi::Env env)
    {
        auto* nativeInput = new NativeInput(env);
        return *nativeInput;
    }

    void NativeInput::PointerDown(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y)
    {
        m_impl->PointerDown(pointerId, buttonIndex, x, y);
    }

    void NativeInput::PointerUp(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y)
    {
        m_impl->PointerUp(pointerId, buttonIndex, x, y);
    }

    void NativeInput::PointerMove(uint32_t pointerId, uint32_t x, uint32_t y)
    {
        m_impl->PointerMove(pointerId, x, y);
    }

    NativeInput::Impl& NativeInput::Impl::GetFromJavaScript(Napi::Env env)
    {
        return *env.Global().Get(JS_NATIVE_INPUT_NAME).As<Napi::External<NativeInput::Impl>>().Data();
    }

    void NativeInput::Impl::PointerDown(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y)
    {
        m_runtimeScheduler([pointerId, buttonIndex, x, y, this]() {
            const std::string deviceId{GetPointerDeviceId(pointerId)};
            const uint32_t inputIndex{GetPointerButtonInputIndex(buttonIndex)};
            std::vector<int>& deviceInputs{GetOrCreateInputMap(deviceId, { inputIndex, POINTER_X_INPUT_INDEX, POINTER_Y_INPUT_INDEX })};
            deviceInputs[inputIndex] = 1;
            deviceInputs[POINTER_X_INPUT_INDEX] = x;
            deviceInputs[POINTER_Y_INPUT_INDEX] = y;
        });
    }

    void NativeInput::Impl::PointerUp(uint32_t pointerId, uint32_t buttonIndex, uint32_t x, uint32_t y)
    {
        m_runtimeScheduler([pointerId, buttonIndex, x, y, this]() {
            const std::string deviceId{GetPointerDeviceId(pointerId)};
            const uint32_t inputIndex{GetPointerButtonInputIndex(buttonIndex)};
            std::vector<int>& deviceInputs{GetOrCreateInputMap(deviceId, { inputIndex, POINTER_X_INPUT_INDEX, POINTER_Y_INPUT_INDEX })};
            deviceInputs[inputIndex] = 0;
            deviceInputs[POINTER_X_INPUT_INDEX] = x;
            deviceInputs[POINTER_Y_INPUT_INDEX] = y;

            for (int32_t index = 0; index < deviceInputs.size(); index++)
            {
                if (index != POINTER_X_INPUT_INDEX && index != POINTER_Y_INPUT_INDEX && deviceInputs[index] > 0)
                {
                    return;
                }
            }

            RemoveInputMap(deviceId);
        });
    }

    void NativeInput::Impl::PointerMove(uint32_t pointerId, uint32_t x, uint32_t y)
    {
        m_runtimeScheduler([pointerId, x, y, this]() {
            const std::string deviceId{GetPointerDeviceId(pointerId)};
            std::vector<int>& deviceInputs{GetOrCreateInputMap(deviceId, { POINTER_X_INPUT_INDEX, POINTER_Y_INPUT_INDEX })};
            deviceInputs[POINTER_X_INPUT_INDEX] = x;
            deviceInputs[POINTER_Y_INPUT_INDEX] = y;
        });
    }

    NativeInput::Impl::DeviceStatusChangedCallbackTicket NativeInput::Impl::AddDeviceConnectedCallback(NativeInput::Impl::DeviceStatusChangedCallback&& callback)
    {
        return m_deviceConnectedCallbacks.Insert(std::move(callback));
    }

    NativeInput::Impl::DeviceStatusChangedCallbackTicket NativeInput::Impl::AddDeviceDisconnectedCallback(NativeInput::Impl::DeviceStatusChangedCallback&& callback)
    {
        return m_deviceDisconnectedCallbacks.Insert(std::move(callback));
    }

    std::optional<int32_t> NativeInput::Impl::PollInput(const std::string& deviceName, uint32_t inputIndex)
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

    std::vector<int32_t>& NativeInput::Impl::GetOrCreateInputMap(const std::string& deviceId, const std::vector<uint32_t>& inputIndices)
    {
        uint32_t inputIndex = *std::max_element(inputIndices.begin(), inputIndices.end());

        auto previousSize = m_inputs.size();
        std::vector<int32_t>& deviceInputs{m_inputs[deviceId]};
        auto newSize = m_inputs.size();

        if (newSize != previousSize)
        {
            m_deviceConnectedCallbacks.ApplyToAll([deviceId](auto& callback) {
                callback(deviceId);
            });
        }

        deviceInputs.resize(std::max(deviceInputs.size(), static_cast<size_t>(inputIndex + 1)));

        return deviceInputs;
    }

    void NativeInput::Impl::RemoveInputMap(const std::string& deviceId)
    {
        if (m_inputs.erase(deviceId))
        {
            m_deviceDisconnectedCallbacks.ApplyToAll([deviceId](auto& callback){
               callback(deviceId);
            });
        }
    }
}

namespace Babylon::Plugins
{
    void NativeInput::Impl::DeviceInputSystem::Initialize(Napi::Env env)
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

    NativeInput::Impl::DeviceInputSystem::DeviceInputSystem(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DeviceInputSystem>{info}
        , m_nativeInput{Babylon::Plugins::NativeInput::Impl::GetFromJavaScript(info.Env())}
        , m_deviceConnectedTicket{m_nativeInput.AddDeviceConnectedCallback([this](const std::string& deviceId) {
            if (!m_onDeviceConnected.IsEmpty())
            {
                Napi::Value napiDeviceId = Napi::String::New(Env(), deviceId);
                m_onDeviceConnected({napiDeviceId});
            }
        })}
        , m_deviceDisconnectedTicket{m_nativeInput.AddDeviceDisconnectedCallback([this](const std::string& deviceId) {
            if (!m_onDeviceDisconnected.IsEmpty())
            {
                Napi::Value napiDeviceId = Napi::String::New(Env(), deviceId);
                m_onDeviceDisconnected({napiDeviceId});
            }
        })}
    {
    }

    Napi::Value NativeInput::Impl::DeviceInputSystem::GetOnDeviceConnected(const Napi::CallbackInfo&)
    {
        return m_onDeviceConnected.Value();
    }

    void NativeInput::Impl::DeviceInputSystem::SetOnDeviceConnected(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_onDeviceConnected = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value NativeInput::Impl::DeviceInputSystem::GetOnDeviceDisconnected(const Napi::CallbackInfo&)
    {
        return m_onDeviceDisconnected.Value();
    }

    void NativeInput::Impl::DeviceInputSystem::SetOnDeviceDisconnected(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_onDeviceDisconnected = Napi::Persistent(value.As<Napi::Function>());
    }

    Napi::Value NativeInput::Impl::DeviceInputSystem::PollInput(const Napi::CallbackInfo& info)
    {
        std::string deviceName = info[0].As<Napi::String>().Utf8Value();
        uint32_t inputIndex = info[1].As<Napi::Number>().Uint32Value();
        try
        {
            std::optional<int32_t> inputValue = m_nativeInput.PollInput(deviceName, inputIndex);
            return inputValue ? Napi::Value::From(Env(), *inputValue) : Env().Null();
        }
        catch (const std::runtime_error& exception)
        {
            return Napi::Value::From(Env(), -1);
            // TODO: Re-enable this
            //throw Napi::Error::New(Env(), exception.what());
        }
    }
}
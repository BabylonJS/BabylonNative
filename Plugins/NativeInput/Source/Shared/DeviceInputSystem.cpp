#include "DeviceInputSystem.h"

namespace Babylon::Plugins
{
    void NativeInput::Impl::DeviceInputSystem::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        static constexpr auto JS_CONSTRUCTOR_NAME = "DeviceInputSystem";

        Napi::Function func{
            DefineClass(
                env,
                JS_CONSTRUCTOR_NAME,
                {
                    InstanceMethod("pollInput", &DeviceInputSystem::PollInput),
                    InstanceMethod("isDeviceAvailable", &DeviceInputSystem::IsDeviceAvailable),
                    InstanceMethod("dispose", &DeviceInputSystem::Dispose),
                })};

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_CONSTRUCTOR_NAME, func);
    }

    // TODO: The JavaScript contract is such that the constructor takes an Engine, but really it should take some kind of view id, which should be passed through to NativeInput::GetFromJavaScript.
    // See https://github.com/BabylonJS/BabylonNative/issues/147
    NativeInput::Impl::DeviceInputSystem::DeviceInputSystem(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DeviceInputSystem>{info}
        , m_nativeInput{*NativeInput::GetFromJavaScript(info.Env()).m_impl}
        , m_deviceConnectedTicket{m_nativeInput.AddDeviceConnectedCallback([this, callback = std::make_shared<Napi::FunctionReference>(Napi::Persistent(info[0].As<Napi::Function>()))](DeviceType deviceType, int32_t deviceSlot) {
            (*callback)({Napi::Value::From(Env(), static_cast<uint32_t>(deviceType)),
                Napi::Value::From(Env(), deviceSlot)});
        })}
        , m_deviceDisconnectedTicket{m_nativeInput.AddDeviceDisconnectedCallback([this, callback = std::make_shared<Napi::FunctionReference>(Napi::Persistent(info[1].As<Napi::Function>()))](DeviceType deviceType, int32_t deviceSlot) {
            (*callback)({Napi::Value::From(Env(), static_cast<uint32_t>(deviceType)),
                Napi::Value::From(Env(), deviceSlot)});
        })}
        , m_InputChangedTicket{m_nativeInput.AddInputChangedCallback([this, callback = std::make_shared<Napi::FunctionReference>(Napi::Persistent(info[2].As<Napi::Function>()))](DeviceType deviceType, int32_t deviceSlot, uint32_t inputIndex, std::optional<int32_t> currentState) {
            (*callback)({Napi::Value::From(Env(), static_cast<uint32_t>(deviceType)),
                Napi::Value::From(Env(), deviceSlot),
                Napi::Value::From(Env(), inputIndex),
                currentState ? Napi::Value::From(Env(), *currentState) : Env().Null()});
        })}
    {
    }

    Napi::Value NativeInput::Impl::DeviceInputSystem::PollInput(const Napi::CallbackInfo& info)
    {
        uint32_t deviceType = info[0].As<Napi::Number>().Uint32Value();
        uint32_t deviceSlot = info[1].As<Napi::Number>().Uint32Value();
        uint32_t inputIndex = info[2].As<Napi::Number>().Uint32Value();
        try
        {
            int32_t inputValue = m_nativeInput.PollInput(static_cast<DeviceType>(deviceType), deviceSlot, inputIndex);
            return Napi::Value::From(Env(), inputValue);
        }
        catch (const std::runtime_error& exception)
        {
            throw Napi::Error::New(Env(), exception.what());
        }
    }

    Napi::Value NativeInput::Impl::DeviceInputSystem::IsDeviceAvailable(const Napi::CallbackInfo& info)
    {
        uint32_t deviceType = info[0].As<Napi::Number>().Uint32Value();

        bool isAvailable = m_nativeInput.IsDeviceAvailable(static_cast<DeviceType>(deviceType));

        return Napi::Value::From(Env(), isAvailable);
    }

    Napi::Value NativeInput::Impl::DeviceInputSystem::Dispose(const Napi::CallbackInfo& info)
    {
        m_onDeviceConnected.Reset();
        m_onDeviceDisconnected.Reset();
        m_onInputChanged.Reset();

        return info.Env().Undefined();
    }
}

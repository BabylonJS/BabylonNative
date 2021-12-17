#include "Window.h"

#include <GraphicsImpl.h>

#include <basen.hpp>
#include <chrono>
#include <iterator>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr auto JS_CLASS_NAME = "Window";
        constexpr auto JS_SET_TIMEOUT_NAME = "setTimeout";
        constexpr auto JS_A_TO_B_NAME = "atob";
        constexpr auto JS_ADD_EVENT_LISTENER_NAME = "addEventListener";
        constexpr auto JS_REMOVE_EVENT_LISTENER_NAME = "removeEventListener";
        constexpr auto JS_DEVICE_PIXEL_RATIO_NAME = "devicePixelRatio";
    }

    void Window::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function constructor = DefineClass(
            env,
            JS_CLASS_NAME,
            {});

        auto global = env.Global();
        auto jsNative = JsRuntime::NativeObject::GetFromJavaScript(env);
        auto jsWindow = constructor.New({});

        jsNative.Set(JS_WINDOW_NAME, jsWindow);

        if (global.Get(JS_SET_TIMEOUT_NAME).IsUndefined())
        {
            global.Set(JS_SET_TIMEOUT_NAME, Napi::Function::New(env, &Window::SetTimeout, JS_SET_TIMEOUT_NAME, Window::Unwrap(jsWindow)));
        }

        if (global.Get(JS_A_TO_B_NAME).IsUndefined())
        {
            global.Set(JS_A_TO_B_NAME, Napi::Function::New(env, &Window::DecodeBase64, JS_A_TO_B_NAME));
        }

        if (global.Get(JS_ADD_EVENT_LISTENER_NAME).IsUndefined())
        {
            global.Set(JS_ADD_EVENT_LISTENER_NAME, Napi::Function::New(env, &Window::AddEventListener, JS_ADD_EVENT_LISTENER_NAME));
        }

        if (global.Get(JS_REMOVE_EVENT_LISTENER_NAME).IsUndefined())
        {
            global.Set(JS_REMOVE_EVENT_LISTENER_NAME, Napi::Function::New(env, &Window::RemoveEventListener, JS_REMOVE_EVENT_LISTENER_NAME));
        }

        if (global.Get(JS_DEVICE_PIXEL_RATIO_NAME).IsUndefined()){
            // Create an accessor to add to the window object to define window.devicePixelRatio
            Napi::Object descriptor{Napi::Object::New(env)};
            descriptor.Set("enumerable", Napi::Value::From(env, true));
            descriptor.Set("get", Napi::Function::New(env, &Window::GetDevicePixelRatio, JS_DEVICE_PIXEL_RATIO_NAME, &jsWindow));
            Napi::Object object{global.Get("Object").As<Napi::Object>()};
            Napi::Function defineProperty{object.Get("defineProperty").As<Napi::Function>()};
            defineProperty.Call(object, {global, Napi::String::New(env, JS_DEVICE_PIXEL_RATIO_NAME), descriptor});
        }
    }

    Window& Window::GetFromJavaScript(Napi::Env env)
    {
        return *Window::Unwrap(JsRuntime::NativeObject::GetFromJavaScript(env).Get(JS_WINDOW_NAME).As<Napi::Object>());
    }

    Window::Window(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Window>{info}
        , m_runtime{JsRuntime::GetFromJavaScript(info.Env())}
    {
    }

    void Window::SetTimeout(const Napi::CallbackInfo& info)
    {
        auto function = Napi::Persistent(info[0].As<Napi::Function>());
        auto milliseconds = std::chrono::milliseconds{info[1].As<Napi::Number>().Int32Value()};

        auto& window = *static_cast<Window*>(info.Data());

        window.RecursiveWaitOrCall(std::make_shared<Napi::FunctionReference>(std::move(function)), std::chrono::system_clock::now() + milliseconds);
    }

    Napi::Value Window::DecodeBase64(const Napi::CallbackInfo& info)
    {
        std::string encodedData = info[0].As<Napi::String>().Utf8Value();
        std::u16string decodedData;
        bn::decode_b64(encodedData.begin(), encodedData.end(), std::back_inserter(decodedData));
        return Napi::Value::From(info.Env(), decodedData);
    }

    void Window::AddEventListener(const Napi::CallbackInfo& /*info*/)
    {
        // TODO: handle events
    }

    void Window::RemoveEventListener(const Napi::CallbackInfo& /*info*/)
    {
        // TODO: handle events
    }

    void Window::RecursiveWaitOrCall(
        std::shared_ptr<Napi::FunctionReference> function,
        std::chrono::system_clock::time_point whenToRun)
    {
        m_runtime.Dispatch([this, function = std::move(function), whenToRun](Napi::Env) {
            if (std::chrono::system_clock::now() >= whenToRun)
            {
                function->Call({});
            }
            else
            {
                RecursiveWaitOrCall(std::move(function), whenToRun);
            }
        });
    }

    Napi::Value Window::GetDevicePixelRatio(const Napi::CallbackInfo& info)
    {
        auto env{info.Env()};
        return Napi::Value::From(env, GraphicsImpl::GetFromJavaScript(env).GetDevicePixelRatio());
    }
}

namespace Babylon::Polyfills::Window
{
    void Initialize(Napi::Env env)
    {
        Internal::Window::Initialize(env);
    }
}

#include "NativeWindow.h"

namespace babylon
{
    Napi::Object NativeWindow::Create(Napi::Env& env, void* windowPtr, size_t width, size_t height)
    {
        constexpr auto JS_CLASS_NAME = "NativeWindow";

        Napi::HandleScope scope{ env };

        Napi::Function constructor = DefineClass(
            env,
            JS_CLASS_NAME,
            {
                InstanceAccessor("width", &NativeWindow::GetWidth, nullptr),
                InstanceAccessor("height", &NativeWindow::GetHeight, nullptr)
            });

        return constructor.New({ Napi::External<void>::New(env, windowPtr), Napi::Number::From(env, width), Napi::Number::From(env, height) });
    }

    NativeWindow::NativeWindow(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<NativeWindow>{ info }
        , m_windowPtr{ info[0].As<Napi::External<void>>().Data() }
        , m_width{ static_cast<size_t>(info[1].As<Napi::Number>().Uint32Value()) }
        , m_height{ static_cast<size_t>(info[2].As<Napi::Number>().Uint32Value()) }
        , m_jsWidth{ Napi::Persistent(Napi::Value::From(info.Env(), m_width)) }
        , m_jsHeight{ Napi::Persistent(Napi::Value::From(info.Env(), m_height)) }
    {}

    void NativeWindow::Resize(size_t newWidth, size_t newHeight)
    {
        if (newWidth != m_width || newHeight != m_height)
        {
            m_width = newWidth;
            m_height = newHeight;

            m_jsWidth = Napi::Persistent(Napi::Value::From(m_jsWidth.Env(), m_width));
            m_jsHeight = Napi::Persistent(Napi::Value::From(m_jsHeight.Env(), m_height));

            for (const auto& callback : m_onResizeCallbacks)
            {
                callback(m_width, m_height);
            }
        }
    }

    NativeWindow::OnResizeCallbackTicket NativeWindow::AddOnResizeCallback(OnResizeCallback&& callback)
    {
        std::lock_guard guard{ m_mutex };
        return m_onResizeCallbacks.insert(callback, m_mutex);
    }

    void* NativeWindow::GetWindowPtr() const
    {
        return m_windowPtr;
    }

    size_t NativeWindow::GetWidth() const
    {
        return m_width;
    }

    size_t NativeWindow::GetHeight() const
    {
        return m_height;
    }

    Napi::Value NativeWindow::GetWidth(const Napi::CallbackInfo& info)
    {
        return m_jsWidth.Value();
    }

    Napi::Value NativeWindow::GetHeight(const Napi::CallbackInfo& info)
    {
        return m_jsHeight.Value();
    }
}

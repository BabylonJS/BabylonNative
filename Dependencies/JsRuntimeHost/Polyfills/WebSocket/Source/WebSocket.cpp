#include "WebSocket.h"
#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    void WebSocket::Initialize(Napi::Env env)
    {
        static constexpr auto JS_WEB_SOCKET_CONSTRUCTOR_NAME = "WebSocket";

        if (env.Global().Get(JS_WEB_SOCKET_CONSTRUCTOR_NAME).IsUndefined())
        {
            Napi::Function func = DefineClass(
                env,
                JS_WEB_SOCKET_CONSTRUCTOR_NAME,
                {
                    StaticValue("CONNECTING", Napi::Value::From(env, 0)),
                    StaticValue("OPEN", Napi::Value::From(env, 1)),
                    StaticValue("CLOSING", Napi::Value::From(env, 2)),
                    StaticValue("CLOSED", Napi::Value::From(env, 3)),
                    InstanceAccessor("readyState", &WebSocket::GetReadyState, nullptr),
                    InstanceAccessor("url", &WebSocket::GetURL, nullptr),
                    InstanceAccessor("onopen", &WebSocket::GetOnOpen, &WebSocket::SetOnOpen),
                    InstanceAccessor("onclose", &WebSocket::GetOnClose, &WebSocket::SetOnClose),
                    InstanceAccessor("onmessage", &WebSocket::GetOnMessage, &WebSocket::SetOnMessage),
                    InstanceAccessor("onerror", &WebSocket::GetOnError, &WebSocket::SetOnError),
                    InstanceMethod("close", &WebSocket::Close),
                    InstanceMethod("send", &WebSocket::Send),
                });

            env.Global().Set(JS_WEB_SOCKET_CONSTRUCTOR_NAME, func);
        }
    }

    WebSocket::WebSocket(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<WebSocket>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
        , m_webSocket(
              info[0].As<Napi::String>(),
              [this] { OpenCallback(); },
              [this](int code, const std::string& reason) { CloseCallback(code, reason); },
              [this](const std::string& message) { MessageCallback(message); },
              [this](const std::string& message) { ErrorCallback(message); })
        , m_url(info[0].As<Napi::String>())
        , m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
    {
        m_webSocket.Open();
    }

    WebSocket::~WebSocket()
    {
        m_cancellationSource->cancel();
    }

    void WebSocket::Close(const Napi::CallbackInfo& info)
    {
        if (m_readyState == ReadyState::Closed || m_readyState == ReadyState::Closing)
        {
            throw Napi::Error::New(info.Env(), "Close has already been called.");
        }
        m_readyState = ReadyState::Closing;
        m_webSocket.Close();
    }

    void WebSocket::Send(const Napi::CallbackInfo& info)
    {
        if (m_readyState != ReadyState::Open)
        {
            throw Napi::Error::New(info.Env(), "Websocket readyState is not open.");
        }
        std::string message = info[0].As<Napi::String>();
        m_webSocket.Send(message);
    }

    Napi::Value WebSocket::GetReadyState(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), arcana::underlying_cast(m_readyState));
    }

    Napi::Value WebSocket::GetURL(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_url);
    }

    void WebSocket::SetOnOpen(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (value.IsNull() || value.IsUndefined())
        {
            m_onopen.Reset();
        }
        else
        {
            m_onopen = Napi::Persistent(value.As<Napi::Function>());
        }
    }

    void WebSocket::SetOnClose(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (value.IsNull() || value.IsUndefined())
        {
            m_onclose.Reset();
        }
        else
        {
            m_onclose = Napi::Persistent(value.As<Napi::Function>());
        }
    }

    void WebSocket::SetOnMessage(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (value.IsNull() || value.IsUndefined())
        {
            m_onmessage.Reset();
        }
        else
        {
            m_onmessage = Napi::Persistent(value.As<Napi::Function>());
        }
    }

    void WebSocket::SetOnError(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (value.IsNull() || value.IsUndefined())
        {
            m_onerror.Reset();
        }
        else
        {
            m_onerror = Napi::Persistent(value.As<Napi::Function>());
        }
    }

    Napi::Value WebSocket::GetOnOpen(const Napi::CallbackInfo&)
    {
        if (m_onopen.IsEmpty())
        {
            return Napi::Value::From(Env(), Env().Null());
        }

        return Napi::Value::From(Env(), m_onopen.Value());
    }

    Napi::Value WebSocket::GetOnClose(const Napi::CallbackInfo&)
    {
        if (m_onclose.IsEmpty())
        {
            return Napi::Value::From(Env(), Env().Null());
        }

        return Napi::Value::From(Env(), m_onclose.Value());
    }

    Napi::Value WebSocket::GetOnMessage(const Napi::CallbackInfo&)
    {
        if (m_onmessage.IsEmpty())
        {
            return Napi::Value::From(Env(), Env().Null());
        }

        return Napi::Value::From(Env(), m_onmessage.Value());
    }

    Napi::Value WebSocket::GetOnError(const Napi::CallbackInfo&)
    {
        if (m_onerror.IsEmpty())
        {
            return Napi::Value::From(Env(), Env().Null());
        }

        return Napi::Value::From(Env(), m_onerror.Value());
    }

    void WebSocket::OpenCallback()
    {
        m_runtimeScheduler([this, cancellationSource{m_cancellationSource}]() {
            if (cancellationSource->cancelled())
            {
                return;
            }
            m_readyState = ReadyState::Open;
            try
            {
                if (!m_onopen.IsEmpty())
                {
                    m_onopen.Call({});
                }
            }
            catch (...)
            {
                Napi::Error::New(Env(), std::current_exception())
                    .ThrowAsJavaScriptException();
            }
        });
    };

    void WebSocket::CloseCallback(int code, const std::string& reason)
    {
        m_runtimeScheduler([this, code, reason, cancellationSource{m_cancellationSource}]() {
            if (cancellationSource->cancelled())
            {
                return;
            }
            m_readyState = ReadyState::Closed;
            try
            {
                Napi::Object closeEvent = Napi::Object::New(Env());
                closeEvent.Set("code", code);
                closeEvent.Set("reason", reason);
                if (!m_onclose.IsEmpty())
                {
                    m_onclose.Call({closeEvent});
                }
            }
            catch (...)
            {
                Napi::Error::New(Env(), std::current_exception())
                    .ThrowAsJavaScriptException();
            }

            m_onopen.Reset();
            m_onclose.Reset();
            m_onmessage.Reset();
            m_onerror.Reset();
        });
    }

    void WebSocket::MessageCallback(const std::string& message)
    {
        m_runtimeScheduler([this, message, cancellationSource{m_cancellationSource}]() {
            if (cancellationSource->cancelled())
            {
                return;
            }
            try
            {
                if (!m_onmessage.IsEmpty())
                {
                    Napi::Object messageEvent = Napi::Object::New(Env());
                    messageEvent.Set("data", message);
                    if (!cancellationSource->cancelled())
                    {
                        m_onmessage.Call({messageEvent});
                    }
                }
            }
            catch (...)
            {
                Napi::Error::New(Env(), std::current_exception())
                    .ThrowAsJavaScriptException();
            }
        });
    }

    void WebSocket::ErrorCallback(const std::string& message)
    {
        m_runtimeScheduler([this, message, cancellationSource{m_cancellationSource}]() {
            if (cancellationSource->cancelled())
            {
                return;
            }
            try
            {
                if (!m_onerror.IsEmpty())
                {
                    Napi::Object errorEvent = Napi::Object::New(Env());
                    errorEvent.Set("message", message);
                    if (!cancellationSource->cancelled())
                    {
                        m_onerror.Call({errorEvent});
                    }
                }
            }
            catch (...)
            {
                Napi::Error::New(Env(), std::current_exception())
                    .ThrowAsJavaScriptException();
            }
        });
    }
}

namespace Babylon::Polyfills::WebSocket
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::WebSocket::Initialize(env);
    }
}

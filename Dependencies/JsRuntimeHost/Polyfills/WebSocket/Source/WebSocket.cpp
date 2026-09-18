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
        // Keep the JS wrapper (and therefore this native object) alive for the
        // lifetime of the connection. The socket delivers open/message/error/
        // close notifications asynchronously via m_runtimeScheduler. Without a
        // strong self-reference the wrapper could be garbage collected before
        // those callbacks fire, so they would observe a cancelled cancellation
        // source (or a destroyed `this`) and never run. The reference is
        // released in CloseCallback, the socket's terminal event.
        //
        // A strong Napi::ObjectReference to the wrapper is used (rather than
        // ObjectWrap::Ref()) so this compiles across every N-API backend,
        // including the JSI backend whose ObjectWrap does not derive from
        // Napi::Reference and therefore has no Ref()/Unref()/Value().
        //
        // ---------------------------------------------------------------------
        // Relationship to the WHATWG WebSocket GC rules
        // (https://websockets.spec.whatwg.org/#garbage-collection):
        //
        // The spec says a WebSocket must NOT be garbage collected while:
        //   1. CONNECTING - if an open/message/error/close listener is set.
        //   2. OPEN        - if a message/error/close listener is set
        //                    (an `open` listener no longer counts once open).
        //   3. CLOSING     - if an error/close listener is set.
        //   4. it has an established connection with data queued to transmit.
        // And if a WebSocket IS collected while still open, the user agent must
        // start the closing handshake (send a Close frame). A CLOSED socket has
        // no keep-alive requirement.
        //
        // This unconditional self-reference is a deliberate, conservative
        // OVER-APPROXIMATION of those rules: it pins the wrapper for the entire
        // connection regardless of ready state or which handlers are set. That
        // satisfies every "must not be garbage collected" clause (1-4) by simply
        // never collecting during the connection, and never trips the
        // "collected while open" clause (the socket is never collected while
        // open), so no normative requirement is violated.
        //
        // The trade-off is purely non-normative retention: a socket with no
        // handlers, or an abandoned still-open socket, stays alive until its
        // terminal close (or until runtime teardown, where Napi::Detach force-
        // releases the reference) instead of being collectible as a browser
        // permits - and consequently the spec's "start the closing handshake on
        // GC while open" step is never exercised. Making the pin conditional on
        // (ready state x registered handler) - and starting a closing handshake
        // in the destructor when still OPEN - would match a browser's resource
        // behavior more precisely, at the cost of added cross-thread complexity.
        // This is intentionally left as the simpler, safe over-approximation.
        // ---------------------------------------------------------------------
        m_selfReference = Napi::Persistent(info.This().As<Napi::Object>());
        m_webSocket.Open();
    }

    WebSocket::~WebSocket()
    {
        // Cancel any in-flight scheduler callbacks so they no-op instead of
        // touching this destroyed wrapper. Note this does NOT start a WebSocket
        // closing handshake: per the WHATWG GC rules a socket collected while
        // still OPEN should send a Close frame, but the unconditional self-
        // reference (see the constructor) means the wrapper is never collected
        // while open - it is pinned until its terminal close or until runtime
        // teardown - so that path is intentionally not implemented here.
        m_cancellationSource->cancel();
    }

    void WebSocket::Close(const Napi::CallbackInfo&)
    {
        // Per the WHATWG WebSocket spec, calling close() on a socket that is
        // already CLOSING or CLOSED is a no-op.
        if (m_readyState == ReadyState::Closed || m_readyState == ReadyState::Closing)
        {
            return;
        }
        m_readyState = ReadyState::Closing;
        m_webSocket.Close();
    }

    void WebSocket::Send(const Napi::CallbackInfo& info)
    {
        // Per the WHATWG WebSocket spec, send() throws InvalidStateError only
        // when readyState is CONNECTING. When CLOSING or CLOSED, the data is
        // silently discarded (the spec still bumps bufferedAmount, which this
        // polyfill does not track).
        if (m_readyState == ReadyState::Connecting)
        {
            throw Napi::Error::New(info.Env(), "WebSocket is still in CONNECTING state.");
        }
        if (m_readyState != ReadyState::Open)
        {
            return;
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
            // Move the connection keep-alive established in the constructor into
            // a local strong reference for the duration of this callback. This
            // both releases the keep-alive (Close is the socket's terminal
            // event) and ensures the wrapper is not finalized synchronously
            // (and `this` deleted) while we are still executing - QuickJS frees
            // objects the instant their refcount reaches zero. The wrapper is
            // finalized safely when this local reference goes out of scope at
            // the end of the callback. A moved-from ObjectReference is left
            // empty, so the member no longer keeps the wrapper alive.
            Napi::ObjectReference selfRef = std::move(m_selfReference);
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

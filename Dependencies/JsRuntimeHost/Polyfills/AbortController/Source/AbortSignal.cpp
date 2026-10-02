#include "AbortSignal.h"
#include <Babylon/JsRuntime.h>

namespace Babylon::Polyfills::Internal
{
    void AbortSignal::Initialize(Napi::Env env)
    {
        if (env.Global().Get(JS_ABORT_SIGNAL_CONSTRUCTOR_NAME).IsUndefined())
        {
            Napi::Function func = DefineClass(
                env,
                JS_ABORT_SIGNAL_CONSTRUCTOR_NAME,
                {
                    InstanceAccessor("aborted", &AbortSignal::GetAborted, nullptr),
                    InstanceAccessor("reason", &AbortSignal::GetReason, nullptr),
                    InstanceAccessor("onabort", &AbortSignal::GetOnAbort, &AbortSignal::SetOnAbort),
                    InstanceMethod("throwIfAborted", &AbortSignal::ThrowIfAborted),
                    InstanceMethod("addEventListener", &AbortSignal::AddEventListener),
                    InstanceMethod("removeEventListener", &AbortSignal::RemoveEventListener),
                    StaticMethod("abort", &AbortSignal::AbortStatic),
                });

            env.Global().Set(JS_ABORT_SIGNAL_CONSTRUCTOR_NAME, func);
        }
    }

    AbortSignal::AbortSignal(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<AbortSignal>{info}
    {
    }

    Napi::Value AbortSignal::CreateAbortError(Napi::Env env, const char* message)
    {
        // There is no DOMException polyfill, so represent the abort reason as an Error whose `name`
        // is "AbortError" -- the value web code checks (`err.name === "AbortError"`).
        Napi::Error error = Napi::Error::New(env, message);
        error.Set("name", Napi::String::New(env, "AbortError"));
        return error.Value();
    }

    void AbortSignal::Abort(const Napi::Value& reason)
    {
        if (m_aborted)
        {
            return;
        }

        m_aborted = true;

        Napi::Env env = Env();
        const Napi::Value resolvedReason = (reason.IsUndefined() || reason.IsEmpty())
            ? CreateAbortError(env, "The operation was aborted.")
            : reason;
        m_reason = Napi::Persistent(resolvedReason);

        auto onabort = m_onabort.Value();
        if (!onabort.IsNull() && !onabort.IsUndefined())
        {
            onabort.Call({});
        }

        RaiseEvent("abort");
    }

    Napi::Value AbortSignal::AbortStatic(const Napi::CallbackInfo& info)
    {
        Napi::Env env = info.Env();
        Napi::Object signalObject = env.Global().Get(JS_ABORT_SIGNAL_CONSTRUCTOR_NAME).As<Napi::Function>().New({});
        AbortSignal* signal = AbortSignal::Unwrap(signalObject);
        signal->Abort(info.Length() > 0 ? info[0] : env.Undefined());
        return signalObject;
    }

    Napi::Value AbortSignal::GetAborted(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_aborted);
    }

    Napi::Value AbortSignal::GetReason(const Napi::CallbackInfo&)
    {
        if (m_reason.IsEmpty())
        {
            return Env().Undefined();
        }

        return m_reason.Value();
    }

    void AbortSignal::ThrowIfAborted(const Napi::CallbackInfo& info)
    {
        if (m_aborted)
        {
            throw Napi::Error{info.Env(), GetReason(info)};
        }
    }

    Napi::Value AbortSignal::GetOnAbort(const Napi::CallbackInfo&)
    {
        if (m_onabort.IsEmpty())
        {
            return Napi::Value::From(Env(), Env().Null());
        }

        return Napi::Value::From(Env(), m_onabort.Value());
    }

    void AbortSignal::SetOnAbort(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        if (value.IsNull() || value.IsUndefined())
        {
            m_onabort.Reset();
        }
        else
        {
            m_onabort = Napi::Persistent(value.As<Napi::Function>());
        }
    }

    void AbortSignal::AddEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();

        const auto& eventHandlerRefs = m_eventHandlerRefs[eventType];
        for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
        {
            if (it->Value() == eventHandler)
            {
                throw Napi::Error::New(info.Env(), "Cannot add the same event handler twice");
            }
        }

        m_eventHandlerRefs[eventType].push_back(Napi::Persistent(eventHandler));
    }

    void AbortSignal::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function eventHandler = info[1].As<Napi::Function>();
        auto itType = m_eventHandlerRefs.find(eventType);
        if (itType != m_eventHandlerRefs.end())
        {
            auto& eventHandlerRefs = itType->second;
            for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
            {
                if (it->Value() == eventHandler)
                {
                    eventHandlerRefs.erase(it);
                    break;
                }
            }
        }
    }

    void AbortSignal::RaiseEvent(const char* eventType)
    {
        auto it = m_eventHandlerRefs.find(eventType);
        if (it != m_eventHandlerRefs.end())
        {
            const auto& eventHandlerRefs = it->second;
            for (const auto& eventHandlerRef : eventHandlerRefs)
            {
                eventHandlerRef.Call({});
            }
        }
    }
}

namespace Babylon::Polyfills::AbortSignal
{
    void Initialize(Napi::Env env)
    {
        Internal::AbortSignal::Initialize(env);
    }
}

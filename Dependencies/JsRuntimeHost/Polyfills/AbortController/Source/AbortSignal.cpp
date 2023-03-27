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
                    InstanceAccessor("aborted", &AbortSignal::GetAborted, &AbortSignal::SetAborted),
                    InstanceAccessor("onabort", &AbortSignal::GetOnAbort, &AbortSignal::SetOnAbort),
                    InstanceMethod("addEventListener", &AbortSignal::AddEventListener),
                    InstanceMethod("removeEventListener", &AbortSignal::RemoveEventListener),
                });

            env.Global().Set(JS_ABORT_SIGNAL_CONSTRUCTOR_NAME, func);
        }
    }

    AbortSignal::AbortSignal(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<AbortSignal>{info}
    {
    }

    Napi::Value AbortSignal::GetAborted(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_aborted);
    }

    void AbortSignal::SetAborted(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_aborted = value.As<Napi::Boolean>();
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
}

namespace Babylon::Polyfills::AbortSignal
{
    void Initialize(Napi::Env env)
    {
        Internal::AbortSignal::Initialize(env);
    }
}

#include "AbortController.h"
#include <cassert>

namespace Babylon::Polyfills::Internal
{
    void AbortController::Initialize(Napi::Env env)
    {
        static constexpr auto JS_ABORT_CONTROLLER_CONSTRUCTOR_NAME = "AbortController";
        if (env.Global().Get(JS_ABORT_CONTROLLER_CONSTRUCTOR_NAME).IsUndefined())
        {
            Napi::Function func = DefineClass(
                env,
                JS_ABORT_CONTROLLER_CONSTRUCTOR_NAME,
                {
                    InstanceAccessor("signal", &AbortController::GetSignal, nullptr),
                    InstanceMethod("abort", &AbortController::Abort),
                });

            env.Global().Set(JS_ABORT_CONTROLLER_CONSTRUCTOR_NAME, func);
        }
    }

    Napi::Value AbortController::GetSignal(const Napi::CallbackInfo&)
    {
        return m_signal.Value();
    }

    void AbortController::Abort(const Napi::CallbackInfo&)
    {
        AbortSignal* sig = AbortSignal::Unwrap(m_signal.Value());
        
        assert(sig != nullptr);
        sig->Abort();
    }

    AbortController::AbortController(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<AbortController>{info}
    {
        m_signal = Napi::Persistent(info.Env().Global().Get(AbortSignal::JS_ABORT_SIGNAL_CONSTRUCTOR_NAME).As<Napi::Function>().New({}));
    }
}

namespace Babylon::Polyfills::AbortController
{
    void Initialize(Napi::Env env)
    {
        Internal::AbortController::Initialize(env);
        Internal::AbortSignal::Initialize(env);
    }
}
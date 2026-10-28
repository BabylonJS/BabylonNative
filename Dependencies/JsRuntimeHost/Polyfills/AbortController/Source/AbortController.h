#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>
#include "AbortSignal.h"

namespace Babylon::Polyfills::Internal
{
    class AbortController final : public Napi::ObjectWrap<AbortController>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit AbortController(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetSignal(const Napi::CallbackInfo& info);

        void Abort(const Napi::CallbackInfo& info);
        Napi::ObjectReference m_signal;
    };
}
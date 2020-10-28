#pragma once

#include <Babylon/JsRuntime.h>

#include <arcana/threading/cancellation.h>

namespace Babylon
{
    struct JsRuntime::InternalState
    {
        static auto& GetFromJavaScript(Napi::Env env)
        {
            return *JsRuntime::GetFromJavaScript(env).m_internalState;
        }

        arcana::cancellation_source Cancellation{};
    };
}

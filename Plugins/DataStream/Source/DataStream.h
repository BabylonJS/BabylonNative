#pragma once
#include <napi/env.h>

#include <Babylon/JsRuntime.h>

namespace Babylon::Plugins::Internal
{
    namespace DataStream
    {
        static void Initialize(Napi::Env env);
        Napi::Value UnzipSync(const Napi::CallbackInfo& info);
    };
} // namespace

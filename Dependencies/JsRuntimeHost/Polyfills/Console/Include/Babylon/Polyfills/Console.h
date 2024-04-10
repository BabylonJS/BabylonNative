#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

namespace Babylon::Polyfills::Console
{
    /**
     * Importance level of messages sent via logging callbacks.
     */
    enum class LogLevel
    {
        Log,
        Warn,
        Error,
    };

    using CallbackT = std::function<void BABYLON_API (const char*, LogLevel)>;

    void BABYLON_API Initialize(Napi::Env env, CallbackT callback);
}

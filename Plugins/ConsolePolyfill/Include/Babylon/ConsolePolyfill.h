#pragma once

#include <napi/env.h>

namespace Babylon::ConsolePolyfill
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

    using CallbackT = std::function<void(const char*, LogLevel)>;

    void InitializeAndCreateInstance(Napi::Env env, CallbackT callback);
}

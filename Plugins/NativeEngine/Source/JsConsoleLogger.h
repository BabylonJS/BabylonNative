#pragma once

#include <napi/env.h>

#include <functional>
#include <mutex>
#include <string>

namespace Babylon
{
    /**
     * Utility struct to log messages to JS console.
     */
    struct JsConsoleLogger final
    {
        JsConsoleLogger() = delete;

        static void LogInfo(Napi::Env env, const char* message);
        static void LogWarn(Napi::Env env, const char* message);
        static void LogError(Napi::Env env, const char* message);
    };
}
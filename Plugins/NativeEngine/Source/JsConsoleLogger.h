#pragma once

#include <napi/env.h>

namespace Babylon
{
    struct JsConsoleLogger final
    {
        JsConsoleLogger() = delete;

        static void LogInfo(Napi::Env env, const char* message);
        static void LogWarn(Napi::Env env, const char* message);
        static void LogError(Napi::Env env, const char* message);
    };
}
#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Polyfills::Internal
{
    namespace MeasureText
    {
        Napi::Value CreateInstance(Napi::Env env, Context* context, const std::string& text);
        Napi::Value GetTextMetrics(Napi::Env env, Context* context);
    }
}
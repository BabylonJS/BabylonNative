#pragma once

#include <Babylon/Polyfills/Canvas.h>

namespace Babylon::Plugins::Internal
{
    namespace MeasureTextDawn
    {
        Napi::Value CreateInstance(Napi::Env env, ContextDawn* context, const std::string& text);
    }
}
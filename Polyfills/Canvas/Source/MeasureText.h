#pragma once

#include <Babylon/Polyfills/Canvas.h>

struct NVGcontext;

namespace Babylon::Polyfills::Internal
{
    class Context;

    namespace MeasureText
    {
        Napi::Value CreateInstance(Napi::Env env, Context* context, const std::string& text);
        Napi::Value CreateInstance(Napi::Env env, NVGcontext* context, const std::string& text);
    }
}
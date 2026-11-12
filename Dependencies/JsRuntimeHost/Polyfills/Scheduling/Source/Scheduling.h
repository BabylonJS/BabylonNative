#pragma once

#include "TimeoutDispatcher.h"

#include <Babylon/JsRuntime.h>

#include <optional>

namespace Babylon::Polyfills::Internal
{
    class Scheduling : public Napi::ObjectWrap<Scheduling>
    {
    public:
        static void Initialize(Napi::Env env);
    };
}

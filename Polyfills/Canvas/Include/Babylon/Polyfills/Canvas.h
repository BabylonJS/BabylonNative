#pragma once

#include <napi/env.h>

namespace Babylon
{
    class Graphics;
}

namespace Babylon::Polyfills::Canvas
{
    void Initialize(Napi::Env env);
}

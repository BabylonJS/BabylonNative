#pragma once

#include <napi/env.h>
#include <Babylon/Graphics.h>

namespace Babylon::Polyfills::Window
{
    void Initialize(Napi::Env env, const Graphics& graphics);
}

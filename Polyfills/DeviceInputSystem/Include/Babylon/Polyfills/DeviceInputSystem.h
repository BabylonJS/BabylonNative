#pragma once

#include <napi/env.h>

namespace Babylon::Polyfills::DeviceInputSystem
{
    void Initialize(Napi::Env env);

    // TODO: These calls should specify a view id or something (that maps to a Babylon View), which should then forward to all DeviceInputSystem instances created for that view.
    void PointerDown();
    void PointerUp();
    void PointerMove(int x, int y);
}

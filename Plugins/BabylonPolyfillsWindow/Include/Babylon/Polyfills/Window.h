#pragma once

#include <napi/env.h>

namespace Babylon::Polyfills::Window
{
    void Initialize(Napi::Env env, void* windowPtr, size_t width, size_t height);

    void UpdateSize(Napi::Env env, size_t width, size_t height);
}

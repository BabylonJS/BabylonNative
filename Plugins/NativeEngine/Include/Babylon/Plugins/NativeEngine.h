#pragma once

#include <napi/env.h>

namespace Babylon::Plugins::NativeEngine
{
    void InitializeGraphics(void* windowPtr, size_t width, size_t height, bool theadedRendering);

    void Initialize(Napi::Env env);

    void Reinitialize(Napi::Env env, void* windowPtr, size_t width, size_t height);

    void DeinitializeGraphics();

    void Resize(size_t width, size_t height);
    // when theadedRendering is false, user is responsible for calling Render
    void Render();
}

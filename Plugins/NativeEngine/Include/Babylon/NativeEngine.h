#pragma once

#include <Babylon/JsRuntime.h>

namespace Babylon
{
    void InitializeNativeEngine(JsRuntime& runtime, void* windowPtr, size_t width, size_t height);
}
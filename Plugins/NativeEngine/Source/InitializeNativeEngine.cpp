#include <Babylon/NativeEngine.h>
#include "NativeEngine.h"
#include "NativeXr.h"

namespace Babylon
{
    void InitializeNativeEngine(JsRuntime& runtime, void* windowPtr, size_t width, size_t height)
    {
        NativeEngine::InitializeWindow(windowPtr, width, height);
        runtime.Dispatch([](Napi::Env env)
        {
            NativeEngine::Initialize(env);
            InitializeNativeXr(env);
        });
    }
}
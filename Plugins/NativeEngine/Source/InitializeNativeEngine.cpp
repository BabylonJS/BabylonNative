#include <Babylon/NativeEngine.h>
#include "NativeEngine.h"

#ifdef NATIVE_ENGINE_XR
#include "NativeXr.h"
#endif

namespace Babylon
{
    void InitializeNativeEngine(JsRuntime& runtime, void* windowPtr, size_t width, size_t height)
    {
        NativeEngine::InitializeWindow(windowPtr, width, height);
        runtime.Dispatch([](Napi::Env env)
        {
            NativeEngine::Initialize(env);
#ifdef NATIVE_ENGINE_XR
            InitializeNativeXr(env);
#endif
        });
    }
}

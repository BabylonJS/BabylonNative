#include "RuntimeImpl.h"

#define USE_EDGEMODE_JSRT
#include <jsrt.h>
#undef USE_EDGEMODE_JSRT

#include <napi/env.h>

namespace Babylon
{
    namespace
    {
        using DispatchFunction = std::function<void(std::function<void()>)>;

        void ThrowIfFailed(JsErrorCode errorCode)
        {
            if (errorCode != JsErrorCode::JsNoError)
            {
                throw std::exception();
            }
        }
    }

    void RuntimeImpl::BaseThreadProcedure()
    {
        DispatchFunction dispatchFunction{
            [this](std::function<void()> action) {
                Dispatch([action = std::move(action)](auto&) {
                    action();
                });
            }
        };

        JsRuntimeHandle jsRuntime;
        ThrowIfFailed(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &jsRuntime));
        JsContextRef context;
        ThrowIfFailed(JsCreateContext(jsRuntime, &context));
        ThrowIfFailed(JsSetCurrentContext(context));
        ThrowIfFailed(JsSetPromiseContinuationCallback([](JsValueRef task, void* callbackState)
        {
            ThrowIfFailed(JsAddRef(task, nullptr));
            auto* dispatch = reinterpret_cast<DispatchFunction*>(callbackState);
            dispatch->operator()([task]()
            {
                JsValueRef undefined;
                ThrowIfFailed(JsGetUndefinedValue(&undefined));
                ThrowIfFailed(JsCallFunction(task, &undefined, 1, nullptr));
                ThrowIfFailed(JsRelease(task, nullptr));
            });
        }, &dispatchFunction));
        ThrowIfFailed(JsProjectWinRTNamespace(L"Windows"));

#ifdef _DEBUG
        // Put Chakra in debug mode.
        ThrowIfFailed(JsStartDebugging());
#endif

        Napi::Env env = Napi::Attach();
        RunJavaScript(env);
        Napi::Detach(env);

        ThrowIfFailed(JsSetCurrentContext(JS_INVALID_REFERENCE));
        ThrowIfFailed(JsDisposeRuntime(jsRuntime));
    }
}
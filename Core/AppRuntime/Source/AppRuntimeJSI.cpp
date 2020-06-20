#include "AppRuntime.h"

#include <jsrt.h>

#include <ChakraRuntime.h>

#include <gsl/gsl>
#include <cassert>

namespace Babylon
{
    namespace
    {
        void ThrowIfFailed(JsErrorCode errorCode)
        {
            if (errorCode != JsErrorCode::JsNoError)
            {
                throw std::exception();
            }
        }
    }

    void AppRuntime::RunEnvironmentTier(const char*)
    {
        using DispatchFunction = std::function<void(std::function<void()>)>;
        DispatchFunction dispatchFunction{
            [this](std::function<void()> action) {
                Dispatch([action = std::move(action)](Napi::Env) {
                    action();
                });
            }};

        JsRuntimeHandle jsRuntime;
        ThrowIfFailed(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &jsRuntime));
        JsContextRef context;
        ThrowIfFailed(JsCreateContext(jsRuntime, &context));
        ThrowIfFailed(JsSetCurrentContext(context));
        ThrowIfFailed(JsSetPromiseContinuationCallback([](JsValueRef task, void* callbackState) {
            ThrowIfFailed(JsAddRef(task, nullptr));
            auto* dispatch = reinterpret_cast<DispatchFunction*>(callbackState);
            dispatch->operator()([task]() {
                JsValueRef undefined;
                ThrowIfFailed(JsGetUndefinedValue(&undefined));
                ThrowIfFailed(JsCallFunction(task, &undefined, 1, nullptr));
                ThrowIfFailed(JsRelease(task, nullptr));
            });
        },
            &dispatchFunction));
        ThrowIfFailed(JsProjectWinRTNamespace(L"Windows"));

#ifdef _DEBUG
        // Put Chakra in debug mode.
        ThrowIfFailed(JsStartDebugging());
#endif

        Microsoft::JSI::ChakraRuntime chakraRuntime{jsRuntime, context};

        {
            Napi::Env env = Napi::Attach<facebook::jsi::Runtime&>(chakraRuntime);
            Run(env);
            Napi::Detach(env);
        }

        ThrowIfFailed(JsSetCurrentContext(JS_INVALID_REFERENCE));
        ThrowIfFailed(JsDisposeRuntime(jsRuntime));
    }
}
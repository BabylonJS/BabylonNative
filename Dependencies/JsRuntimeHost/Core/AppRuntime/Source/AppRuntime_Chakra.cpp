#include "AppRuntime.h"
#include <napi/env.h>

#define USE_EDGEMODE_JSRT
#include <jsrt.h>

#include <gsl/gsl>
#include <cassert>
#include <sstream>

namespace Babylon
{
    namespace
    {
        void ThrowIfFailed(JsErrorCode errorCode)
        {
            if (errorCode != JsErrorCode::JsNoError)
            {
                std::ostringstream ss;
                ss << "Chakra function failed with error code (" << static_cast<int>(errorCode) << ")";
                throw std::runtime_error{ss.str()};
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
        ThrowIfFailed(JsSetPromiseContinuationCallback(
            [](JsValueRef task, void* callbackState) {
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

        if (m_options.EnableDebugger)
        {
            auto result = JsStartDebugging();
            if (result != JsErrorCode::JsNoError)
            {
                OutputDebugStringW(L"Failed to initialize JavaScript debugging support.\n");
            }
        }

        Napi::Env env = Napi::Attach();

        Run(env);

        ThrowIfFailed(JsSetCurrentContext(JS_INVALID_REFERENCE));
        ThrowIfFailed(JsDisposeRuntime(jsRuntime));

        // Detach must come after JsDisposeRuntime since it triggers finalizers which require env.
        Napi::Detach(env);
    }
}

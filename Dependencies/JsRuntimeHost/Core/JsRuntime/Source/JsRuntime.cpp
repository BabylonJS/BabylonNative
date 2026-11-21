#include "JsRuntime.h"
#include "Babylon/DebugTrace.h"

#include <mutex>
#include <utility>

namespace Babylon
{
    struct JsRuntime::InternalState
    {
        explicit InternalState(DispatchFunctionT dispatchFunction)
            : DispatchFunction{std::move(dispatchFunction)}
        {
        }

        DispatchFunctionT DispatchFunction;
        std::mutex Mutex;
    };

    namespace
    {
        static constexpr auto JS_RUNTIME_NAME = "runtime";
        static constexpr auto JS_WINDOW_NAME = "window";
    }

    JsRuntime::JsRuntime(Napi::Env env, DispatchFunctionT dispatchFunction)
        : m_state{std::make_shared<InternalState>(std::move(dispatchFunction))}
    {
        auto global = env.Global();

        if (global.Get(JS_WINDOW_NAME).IsUndefined())
        {
            global.Set(JS_WINDOW_NAME, global);
        }

        auto jsNative = Napi::Object::New(env);
        env.Global().Set(NativeObject::JS_NATIVE_NAME, jsNative);

        Napi::Value jsRuntime = Napi::External<JsRuntime>::New(env, this, [](Napi::Env, JsRuntime* runtime) { delete runtime; });
        jsNative.Set(JS_RUNTIME_NAME, jsRuntime);

        DEBUG_TRACE("JsRuntime created");
    }

    JsRuntime::~JsRuntime()
    {
        Close(m_state);
    }

    void JsRuntime::Close(const std::shared_ptr<InternalState>& state)
    {
        DispatchFunctionT dispatchFunction;
        if (state)
        {
            std::scoped_lock lock{state->Mutex};
            dispatchFunction = std::exchange(state->DispatchFunction, {});
        }
        // Captured objects may dispatch from their destructors. Release them unlocked.
    }

    JsRuntime& BABYLON_API JsRuntime::CreateForJavaScript(Napi::Env env, DispatchFunctionT dispatchFunction)
    {
        auto* runtime = new JsRuntime(env, std::move(dispatchFunction));
        return *runtime;
    }

    JsRuntime& BABYLON_API JsRuntime::GetFromJavaScript(Napi::Env env)
    {
        return *NativeObject::GetFromJavaScript(env)
                    .As<Napi::Object>()
                    .Get(JS_RUNTIME_NAME)
                    .As<Napi::External<JsRuntime>>()
                    .Data();
    }

    void JsRuntime::Dispatch(std::function<void BABYLON_API (Napi::Env)> function)
    {
        Dispatch(m_state, std::move(function));
    }

    void JsRuntime::Dispatch(const std::shared_ptr<InternalState>& state, std::function<void BABYLON_API (Napi::Env)> function)
    {
        // Keep the host enqueue and Close mutually exclusive: copying the dispatch
        // function out of the lock would allow it to run after the host is gone.
        std::scoped_lock lock{state->Mutex};
        if (!state->DispatchFunction)
        {
            return;
        }

        state->DispatchFunction([function = std::move(function)](Napi::Env env) {
            function(env);

            // The environment will be in a pending exceptional state if
            // Napi::Error::ThrowAsJavaScriptException is invoked within the
            // previous function. Throw and clear the pending exception here to
            // bubble up the exception to the the dispatcher.
            if (env.IsExceptionPending())
            {
                throw env.GetAndClearPendingException();
            }
        });
    }
}

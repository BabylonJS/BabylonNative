#include <Babylon/Polyfills/Performance.h>

#include <napi/napi.h>
#include <chrono>

namespace
{
    constexpr const char* JS_INSTANCE_NAME{"performance"};
}

namespace Babylon::Polyfills::Performance
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        auto performance = env.Global().Get(JS_INSTANCE_NAME).As<Napi::Object>();
        if (!performance.IsUndefined())
        {
            return; // already defined (might be QuickJS built-in)
        }

        performance = Napi::Object::New(env);
        env.Global().Set(JS_INSTANCE_NAME, performance);

        // The time origin belongs to this environment, as it does to each browsing context and
        // worker: a process-wide origin would be reset by every runtime (worker) that initializes
        // the polyfill, moving performance.now() backwards elsewhere and racing with it.
        const auto timeOrigin = std::chrono::steady_clock::now();
        performance.Set("now", Napi::Function::New(env, [timeOrigin](const Napi::CallbackInfo& info) {
            const auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - timeOrigin);
            return Napi::Number::New(info.Env(), elapsed.count());
        }, "now"));
    }
}

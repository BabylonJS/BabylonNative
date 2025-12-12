#include <Babylon/Polyfills/Performance.h>

#include <napi/napi.h>
#include <chrono>

namespace
{
    constexpr const char* JS_INSTANCE_NAME{"performance"};

    // Store the start time when the polyfill is initialized
    std::chrono::high_resolution_clock::time_point g_startTime;

    Napi::Value Now(const Napi::CallbackInfo& info)
    {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(now - g_startTime);
        return Napi::Number::New(info.Env(), duration.count());
    }
}

namespace Babylon::Polyfills::Performance
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        // Initialize the start time
        g_startTime = std::chrono::high_resolution_clock::now();

        auto performance = env.Global().Get(JS_INSTANCE_NAME).As<Napi::Object>();
        if (performance.IsUndefined())
        {
            performance = Napi::Object::New(env);
            env.Global().Set(JS_INSTANCE_NAME, performance);
        }

        performance.Set("now", Napi::Function::New(env, Now, "now"));
    }
}

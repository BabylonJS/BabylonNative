#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>
#include <future>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/Canvas.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>
#include <chrono>
#include <thread>
#include <napi/env.h>

std::promise<int32_t> exitCode;

static inline constexpr const char* JS_FUNCTION_NAME{ "SetExitCode" };
void SetExitCode(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();
    if (info.Length() < 1)
    {
        throw Napi::TypeError::New(env, "Must provide an exit code");
    }
    exitCode.set_value(info[0].As<Napi::Number>().Int32Value());
}

int Run(std::unique_ptr<Babylon::Graphics> graphics)
{
    std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};
    std::unique_ptr<Babylon::AppRuntime> runtime = std::make_unique<Babylon::AppRuntime>();
    runtime->Dispatch([&graphics, &nativeCanvas](Napi::Env env)
    {
        graphics->AddToJavaScript(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto)
        {
            printf("%s", message);
            fflush(stdout);
        });
        Babylon::Polyfills::Window::Initialize(env);
        nativeCanvas = std::make_unique<Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Plugins::NativeEngine::Initialize(env);
        
        env.Global().Set(JS_FUNCTION_NAME, Napi::Function::New(env, SetExitCode, JS_FUNCTION_NAME));
    });
    Babylon::ScriptLoader loader{*runtime};
    loader.Eval("global = {};", ""); // Required for Chai.js as we do not have global in Babylon Native
    loader.Eval("window.clearTimeout = () => {};", ""); // TODO: implement clear timeout, required for Mocha timeouts to work correctly
    loader.Eval("location = {href: ''};", "");          // Required for Mocha.js as we do not have a location in Babylon Native
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/chai.js");
    loader.LoadScript("app:///Scripts/mocha.js");
    loader.LoadScript("app:///Scripts/tests.js");
    graphics->StartRenderingCurrentFrame();
    graphics->FinishRenderingCurrentFrame();
    auto code{exitCode.get_future().get()};
    return code;
}

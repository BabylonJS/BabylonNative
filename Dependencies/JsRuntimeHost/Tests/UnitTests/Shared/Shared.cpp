#include "Shared.h"
#include <Babylon/AppRuntime.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Polyfills/Scheduling.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <future>

int RunTests(Babylon::Polyfills::Console::CallbackT consoleCallback)
{
    std::promise<int32_t> exitCode;

    Babylon::AppRuntime runtime{[&exitCode](const std::exception& ex) {
        exitCode.set_value(-1);
    }};

    runtime.Dispatch([&exitCode, consoleCallback = std::move(consoleCallback)](Napi::Env env) mutable
    {
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, std::move(consoleCallback));
        Babylon::Polyfills::Scheduling::Initialize(env);

        env.Global().Set("SetExitCode", Napi::Function::New(env, [&exitCode](const Napi::CallbackInfo& info)
        {
            Napi::Env env = info.Env();
            exitCode.set_value(info[0].As<Napi::Number>().Int32Value());
        }, "SetExitCode"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.Eval("var global = {};", ""); // Required for chai.js
    loader.Eval("var location = { href: '' };", ""); // Required for mocha.js
    loader.LoadScript("app:///Scripts/chai.js");
    loader.LoadScript("app:///Scripts/mocha.js");
    loader.LoadScript("app:///Scripts/tests.js");

    return exitCode.get_future().get();
}

#include <UrlLib/UrlLib.h>
#include <arcana/threading/task.h>
#include <future>
#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>
#include <chrono>
#include <thread>

std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<Babylon::Graphics> graphics{};

int run()
{
    Babylon::ContextConfiguration graphicsConfig{};
    graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);

    runtime = std::make_unique<Babylon::AppRuntime>();
    runtime->Dispatch([](Napi::Env env)
    {
        graphics->AddToJavaScript(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            printf("%s", message);
            fflush(stdout);
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
    });
    Babylon::ScriptLoader loader{*runtime};
    loader.Eval("global = {};", ""); // Required for Chai.js as we do not have global in Babylon Native
    loader.Eval("window.clearTimeout = () => {};", ""); // TODO: implement clear timeout, required for Mocha timeouts to work correctly
    loader.Eval("location = {href: ''};", "");          // Required for Mocha.js as we do not have a location in Babylon Native
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("https://unpkg.com/chai/chai.js");
    loader.LoadScript("https://unpkg.com/mocha/mocha.js");
    loader.LoadScript("app:///Scripts/tests.js");
    // TODO: add mechanism to wait until an exit code is received from tests.js
    std::this_thread::sleep_for(std::chrono::seconds(1000000));
}

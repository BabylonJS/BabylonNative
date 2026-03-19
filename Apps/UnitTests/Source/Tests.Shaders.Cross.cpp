#include <gtest/gtest.h>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/ScriptLoader.h>

#include <chrono>
#include <future>
#include <iostream>

using namespace std::chrono_literals;

extern Babylon::Graphics::Configuration g_deviceConfig;

TEST(ShadersCross, CompileComprehensiveGLSL)
{
    Babylon::Graphics::Device device{g_deviceConfig};
    Babylon::Graphics::DeviceUpdate update{device.GetUpdate("update")};

    device.StartRenderingCurrentFrame();
    update.Start();

    std::promise<void> scriptIsDone{};
    std::promise<int32_t> shaderTestDone{};

    Babylon::AppRuntime::Options options{};

    options.UnhandledExceptionHandler = [&scriptIsDone](const Napi::Error& error) {
        std::cerr << "[Uncaught Error] " << Napi::GetErrorString(error) << std::endl;
        std::cerr.flush();

        scriptIsDone.set_exception(std::make_exception_ptr(std::exception{}));
    };

    Babylon::AppRuntime runtime{options};
    runtime.Dispatch([&device, &shaderTestDone](Napi::Env env) {
        device.AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            std::cout << message << std::endl;
            std::cout.flush();
        });
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        env.Global().Set("setShaderTestDone",
            Napi::Function::New(
                env, [&shaderTestDone](const Napi::CallbackInfo& info) {
                    int32_t failures = info[0].As<Napi::Number>().Int32Value();
                    shaderTestDone.set_value(failures);
                },
                "setShaderTestDone"));
    });

    Babylon::ScriptLoader loader{runtime};
    loader.LoadScript("app:///Assets/babylon.max.js");
    loader.LoadScript("app:///Assets/tests.shaders.cross.js");
    loader.Dispatch([&scriptIsDone](Napi::Env) {
        scriptIsDone.set_value();
    });

    scriptIsDone.get_future().get();

    auto shaderTestFuture = shaderTestDone.get_future();
    constexpr auto timeout = 30s;
    auto start = std::chrono::steady_clock::now();
    while (shaderTestFuture.wait_for(16ms) != std::future_status::ready)
    {
        update.Finish();
        device.FinishRenderingCurrentFrame();
        device.StartRenderingCurrentFrame();
        update.Start();

        if (std::chrono::steady_clock::now() - start > timeout)
        {
            std::cerr << "Shader test timed out after " << timeout.count() << "s" << std::endl;
            break;
        }
    }

    auto failures = shaderTestFuture.get();
    std::cout << "Shader cross-compilation test finished with " << failures << " failure(s)." << std::endl;

    update.Finish();
    device.FinishRenderingCurrentFrame();
}

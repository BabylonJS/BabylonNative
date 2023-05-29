#include <filesystem>
#include <stdio.h>
#include <optional>

#include <Babylon/AppRuntime.h>
#include <Babylon/Graphics/Device.h>
#include <Babylon/ScriptLoader.h>
#include <Babylon/Plugins/NativeCapture.h>
#include <Babylon/Plugins/NativeEngine.h>
#include <Babylon/Plugins/NativeOptimizations.h>
#include <Babylon/Plugins/ChromeDevTools.h>
#include <Babylon/Plugins/NativeXr.h>
#include <Babylon/Plugins/NativeCamera.h>
#include <Babylon/Plugins/NativeInput.h>
#include <Babylon/Polyfills/Console.h>
#include <Babylon/Polyfills/Window.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <Babylon/Polyfills/Canvas.h>

std::optional<Babylon::AppRuntime> runtime{};
std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
Babylon::Plugins::NativeInput* nativeInput{};
std::optional<Babylon::Plugins::ChromeDevTools> chromeDevTools{};
std::optional<Babylon::Polyfills::Canvas> nativeCanvas{};

int main(int argc, char** argv)
{
    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = NULL;
    graphicsConfig.Width = 128;
    graphicsConfig.Height = 128;
    graphicsConfig.MSAASamples = 4;

    device.emplace(graphicsConfig);
    update.emplace(device->GetUpdate("update"));

    device->StartRenderingCurrentFrame();
    update->Start();

    runtime.emplace();

    runtime->Dispatch([](Napi::Env env) {
        device->AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            OutputDebugStringA(message);
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        Babylon::Plugins::NativeCapture::Initialize(env);

        Babylon::Plugins::NativeCamera::Initialize(env);

        //Babylon::Plugins::NativeXr::Initialize(env);

        nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);

        chromeDevTools.emplace(Babylon::Plugins::ChromeDevTools::Initialize(env));
        if (chromeDevTools->SupportsInspector())
        {
            chromeDevTools->StartInspector(5643, "Test Install");
        }
    });

    Babylon::ScriptLoader loader{*runtime};
    loader.Eval("console.log(\"Done.\");", "DummyTest");

    return 0;
}
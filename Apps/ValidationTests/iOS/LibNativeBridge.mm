#include "LibNativeBridge.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics/Device.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeOptimizations.h>
#import <Babylon/Plugins/NativeXr.h>
#import <Babylon/Plugins/TestUtils.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Console.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>

#import <UIKit/UIKit.h>

#import <optional>

std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
std::optional<Babylon::AppRuntime> runtime{};
std::optional<Babylon::Polyfills::Canvas> nativeCanvas{};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
    if (device)
    {
        update->Finish();
        device->FinishRenderingCurrentFrame();
    }
    
    nativeCanvas.reset();
    runtime.reset();
    update.reset();
    device.reset();
}

- (void)init:(MTKView*)view width:(int)inWidth height:(int)inHeight
{
    runtime.reset();
    device.reset();

    float width = inWidth;
    float height = inHeight;

    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = view;
    graphicsConfig.Width = static_cast<size_t>(width);
    graphicsConfig.Height = static_cast<size_t>(height);

    device.emplace(graphicsConfig);
    update.emplace(device->GetUpdate("update"));

    device->StartRenderingCurrentFrame();
    device->SetDiagnosticOutput([](const char* outputString) { printf("%s", outputString); fflush(stdout); });
    update->Start();

    runtime.emplace();

    runtime->Dispatch([](Napi::Env env)
    {
        device->AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            NSLog(@"%s", message);
        });

        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        Babylon::Plugins::NativeXr::Initialize(env);

        Babylon::Plugins::TestUtils::Initialize(env, nullptr);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/babylon.gui.js");
    loader.LoadScript("app:///Scripts/validation_native.js");
}

- (void)resize:(int)inWidth height:(int)inHeight
{
    if (device)
    {
        update->Finish();
        device->FinishRenderingCurrentFrame();

        device->UpdateSize(static_cast<size_t>(inWidth), static_cast<size_t>(inHeight));

        device->StartRenderingCurrentFrame();
        update->Start();
    }
}

- (void)render
{
    if (device)
    {
        update->Finish();
        device->FinishRenderingCurrentFrame();
        device->StartRenderingCurrentFrame();
        update->Start();
    }
}

@end

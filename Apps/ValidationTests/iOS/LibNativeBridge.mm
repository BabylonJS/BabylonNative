#include "LibNativeBridge.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics/Device.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeOptimizations.h>
#import <Babylon/Plugins/NativeXr.h>
#import <Babylon/Plugins/TestUtils.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <UIKit/UIKit.h>

std::unique_ptr<Babylon::Graphics::Device> device{};
std::unique_ptr<Babylon::Graphics::Device::Update> update{};
std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
}

- (void)init:(MTKView*)view width:(int)inWidth height:(int)inHeight
{
    runtime.reset();
    device.reset();

    float width = inWidth;
    float height = inHeight;

    Babylon::Graphics::WindowConfiguration graphicsConfig{};
    graphicsConfig.WindowPtr = view;
    graphicsConfig.Width = static_cast<size_t>(width);
    graphicsConfig.Height = static_cast<size_t>(height);
    device = Babylon::Graphics::Device::Create(graphicsConfig);
    update = std::make_unique<Babylon::Graphics::Device::Update>(device->GetUpdate("update"));
    device->StartRenderingCurrentFrame();
    device->SetDiagnosticOutput([](const char* outputString) { printf("%s", outputString); fflush(stdout); });
    update->Start();

    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([](Napi::Env env)
    {
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));

        device->AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        // Initialize NativeXr plugin.
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
        device->UpdateSize(static_cast<size_t>(inWidth), static_cast<size_t>(inHeight));
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

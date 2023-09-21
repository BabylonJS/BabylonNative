#include "LibNativeBridge.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics/Device.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeCamera.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeInput.h>
#import <Babylon/Plugins/NativeOptimizations.h>
#import <Babylon/Plugins/NativeXr.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Console.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>

#import <optional>

std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
std::optional<Babylon::AppRuntime> runtime{};
std::optional<Babylon::Polyfills::Canvas> nativeCanvas{};
std::optional<Babylon::Plugins::NativeXr> nativeXr{};
Babylon::Plugins::NativeInput* nativeInput{};
bool isXrActive{};

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
    
    nativeInput = {};
    nativeXr.reset();
    nativeCanvas.reset();
    runtime.reset();
    update.reset();
    device.reset();
}

- (void)init:(MTKView*)view width:(int)inWidth height:(int)inHeight xrView:(void*)xrView
{
    float width = inWidth;
    float height = inHeight;

    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = view;
    graphicsConfig.Width = static_cast<size_t>(width);
    graphicsConfig.Height = static_cast<size_t>(height);

    device.emplace(graphicsConfig);
    update.emplace(device->GetUpdate("update"));

    device->StartRenderingCurrentFrame();
    update->Start();

    runtime.emplace();

    runtime->Dispatch([xrView](Napi::Env env)
    {
        device->AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            NSLog(@"%s", message);
        });

        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        Babylon::Plugins::NativeCamera::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        nativeXr.emplace(Babylon::Plugins::NativeXr::Initialize(env));
        nativeXr->UpdateWindow(xrView);
        nativeXr->SetSessionStateChangedCallback([](bool isXrActive){ ::isXrActive = isXrActive; });

        nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.LoadScript("app:///Scripts/ammo.js");
    loader.LoadScript("app:///Scripts/recast.js");
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/babylon.gui.js");
    loader.LoadScript("app:///Scripts/experience.js");
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

- (void)setTouchDown:(int)pointerId x:(int)inX y:(int)inY
{
    if (nativeInput != nullptr) {
        nativeInput->TouchDown(pointerId, inX, inY);
    }
}

- (void)setTouchMove:(int)pointerId x:(int)inX y:(int)inY
{
    if (nativeInput != nullptr) {
        nativeInput->TouchMove(pointerId, inX, inY);
    }
}

- (void)setTouchUp:(int)pointerId x:(int)inX y:(int)inY
{
    if (nativeInput != nullptr) {
        nativeInput->TouchUp(pointerId, inX, inY);
    }
}

- (bool)isXRActive
{
    return ::isXrActive;
}

@end

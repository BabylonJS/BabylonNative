#include "LibNativeBridge.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeInput.h>
#import <Babylon/Plugins/NativeXr.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::AppRuntime> runtime{};
Babylon::Plugins::NativeInput* nativeInput{};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
}

- (void)init:(void*)view width:(int)inWidth height:(int)inHeight
{
    nativeInput = nullptr;
    runtime.reset();
    graphics.reset();

    float width = inWidth;
    float height = inHeight;
    void* windowPtr = view;

    graphics = Babylon::Graphics::CreateGraphics(windowPtr, static_cast<size_t>(width), static_cast<size_t>(height));
    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([](Napi::Env env)
    {
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        
        graphics->AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        // Initialize NativeXr plugin.
        Babylon::Plugins::NativeXr::Initialize(env);

        nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.Eval("document = {}", "");
    loader.LoadScript("app:///ammo.js");
    loader.LoadScript("app:///recast.js");
    loader.LoadScript("app:///babylon.max.js");
    loader.LoadScript("app:///babylon.glTF2FileLoader.js");
    loader.LoadScript("app:///babylonjs.materials.js");
    loader.LoadScript("app:///babylon.gui.js");
    loader.LoadScript("app:///experience.js");
}

- (void)resize:(int)inWidth height:(int)inHeight
{
    if (graphics)
    {
        graphics->UpdateSize(static_cast<size_t>(inWidth), static_cast<size_t>(inHeight));
    }
}

- (void)setInputs:(int)x y:(int)y tap:(bool)tap
{
    if (nativeInput != nullptr)
    {
        if (tap)
        {
            nativeInput->TouchDown(0, x, y);
        }

        nativeInput->TouchMove(0, x, y);

        if (!tap)
        {
            nativeInput->TouchUp(0, x, y);
        }
    }
}

@end

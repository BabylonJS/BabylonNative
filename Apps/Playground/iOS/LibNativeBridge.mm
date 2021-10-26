#include "LibNativeBridge.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeInput.h>
#import <Babylon/Plugins/NativeXr.h>
#import <Babylon/Plugins/NativeCamera.h>
#import <Babylon/Plugins/NativeOptimizations.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Console.h>

#import <optional>

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::AppRuntime> runtime{};
Babylon::Plugins::NativeInput* nativeInput{};
std::optional<Babylon::Plugins::NativeXr> g_nativeXr{};
std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};
bool g_isXrActive{};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
}

- (void)init:(MTKView*)view width:(int)inWidth height:(int)inHeight xrView:(void*)xrView
{
    nativeInput = nullptr;
    runtime.reset();
    graphics.reset();

    float width = inWidth;
    float height = inHeight;

    Babylon::WindowConfiguration graphicsConfig{};
    graphicsConfig.WindowPtr = view;
    graphicsConfig.Width = static_cast<size_t>(width);
    graphicsConfig.Height = static_cast<size_t>(height);
    graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);
    runtime = std::make_unique<Babylon::AppRuntime>();
    nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);

    runtime->Dispatch([xrView](Napi::Env env)
    {
        graphics->AddToJavaScript(env);

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            NSLog(@"%s", message);
        });

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        // Initialize NativeXr plugin.
        g_nativeXr.emplace(Babylon::Plugins::NativeXr::Initialize(env));
        g_nativeXr->UpdateWindow(xrView);
        g_nativeXr->SetSessionStateChangedCallback([](bool isXrActive){ g_isXrActive = isXrActive; });

        // Initialize Camera 
        Babylon::Plugins::Camera::Initialize(env);

        InputManager<Babylon::AppRuntime>::Initialize(env, *inputBuffer);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.Eval("document = {}", "");
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
    if (graphics)
    {
        graphics->UpdateSize(static_cast<size_t>(inWidth), static_cast<size_t>(inHeight));
    }
}

- (void)render
{
    if (graphics)
    {
        graphics->StartRenderingCurrentFrame();
        graphics->FinishRenderingCurrentFrame();
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

- (bool)isXRActive
{
    return g_isXrActive;
}

@end

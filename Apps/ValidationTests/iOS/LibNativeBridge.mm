#include "LibNativeBridge.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/GraphicsPlatform.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeXr.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <UIKit/UIKit.h>

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::AppRuntime> runtime{};

#import <Shared/TestUtils.h>

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
    runtime.reset();
    graphics.reset();

    float width = inWidth;
    float height = inHeight;
    void* windowPtr = view;
    
    GraphicsConfiguration graphicsConfig = GraphicsConfiguration();
    graphicsConfig.windowPtr = (__bridge WindowType)windowPtr;
    graphicsConfig.width = static_cast<size_t>(width);
    graphicsConfig.height = static_cast<size_t>(height);
    graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);
    graphics->SetDiagnosticOutput([](const char* outputString) { printf("%s", outputString); fflush(stdout); });

    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([](Napi::Env env)
    {
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        
        graphics->AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env);

        // Initialize NativeXr plugin.
        Babylon::Plugins::NativeXr::Initialize(env);
        
        Babylon::TestUtils::CreateInstance(env, nullptr);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.Eval("document = {}", "");
    loader.LoadScript("app:///ammo.js");
    loader.LoadScript("app:///recast.js");
    loader.LoadScript("app:///babylon.max.js");
    loader.LoadScript("app:///babylon.glTF2FileLoader.js");
    loader.LoadScript("app:///babylonjs.materials.js");
    loader.LoadScript("app:///babylon.gui.js");
    loader.LoadScript("app:///validation_native.js");
}

- (void)resize:(int)inWidth height:(int)inHeight
{
    if (graphics)
    {
        graphics->UpdateSize(static_cast<size_t>(inWidth), static_cast<size_t>(inHeight));
    }
}

@end

#include "LibNativeBridge.h"

#import <Shared/AppContext.h>
#import <Babylon/Plugins/NativeXr.h>
#import <optional>

std::optional<AppContext> appContext{};
std::optional<Babylon::Plugins::NativeXr> nativeXr{};
bool isXrActive{};
float screenScale{1.0f};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
    isXrActive = false;

    nativeXr.reset();
    appContext.reset();
}

- (void)init:(MTKView*)view screenScale:(float)inScreenScale width:(int)inWidth height:(int)inHeight xrView:(void*)xrView
{
    screenScale = inScreenScale;

    appContext.emplace(
        view,
        static_cast<size_t>(inWidth),
        static_cast<size_t>(inHeight),
        [](const char* message) {
            NSLog(@"%s", message);
        },
        [xrView](Napi::Env env) {
            nativeXr.emplace(Babylon::Plugins::NativeXr::Initialize(env));
            nativeXr->UpdateWindow(xrView);
            nativeXr->SetSessionStateChangedCallback([](bool isXrActive){ ::isXrActive = isXrActive; });
        });

    appContext->ScriptLoader().LoadScript("app:///Scripts/experience.js");
}

- (void)resize:(int)inWidth height:(int)inHeight
{
    if (appContext)
    {
        appContext->DeviceUpdate().Finish();
        appContext->Device().FinishRenderingCurrentFrame();

        appContext->Device().UpdateSize(static_cast<size_t>(inWidth), static_cast<size_t>(inHeight));

        appContext->Device().StartRenderingCurrentFrame();
        appContext->DeviceUpdate().Start();
    }
}

- (void)render
{
    if (appContext)
    {
        appContext->DeviceUpdate().Finish();
        appContext->Device().FinishRenderingCurrentFrame();
        appContext->Device().StartRenderingCurrentFrame();
        appContext->DeviceUpdate().Start();
    }
}

- (void)setTouchDown:(int)pointerId x:(int)inX y:(int)inY
{
    if (appContext && appContext->Input()) {
        appContext->Input()->TouchDown(pointerId, inX * screenScale, inY * screenScale);
    }
}

- (void)setTouchMove:(int)pointerId x:(int)inX y:(int)inY
{
    if (appContext && appContext->Input()) {
        appContext->Input()->TouchMove(pointerId, inX * screenScale, inY * screenScale);
    }
}

- (void)setTouchUp:(int)pointerId x:(int)inX y:(int)inY
{
    if (appContext && appContext->Input()) {
        appContext->Input()->TouchUp(pointerId, inX * screenScale, inY * screenScale);
    }
}

- (bool)isXRActive
{
    return ::isXrActive;
}

@end

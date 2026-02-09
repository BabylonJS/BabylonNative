#include "LibNativeBridge.h"

#import <Shared/AppContext.h>
#import <Babylon/Plugins/NativeInput.h>
#import <optional>

std::optional<AppContext> appContext{};
float screenScale{1.0f};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
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
        });

    appContext->Runtime().Dispatch([](Napi::Env env) {
        Napi::HandleScope scope{env};

        auto statusCallback = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
            if (info.Length() > 0)
            {
                std::string message{};
                if (info[0].IsString())
                {
                    message = info[0].As<Napi::String>().Utf8Value();
                }
                else
                {
                    message = info[0].ToString().Utf8Value();
                }
                NSLog(@"[Playground] %s", message.c_str());
            }
        });
        env.Global().Set("__nativePlaygroundStatus", statusCallback);
    });

    appContext->ScriptLoader().LoadScript("app:///Scripts/webgpu_smoke.js");
    appContext->ScriptLoader().LoadScript("app:///Scripts/playground_runner.js");
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
    return false;
}

@end

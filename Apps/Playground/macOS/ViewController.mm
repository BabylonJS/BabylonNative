#import "ViewController.h"

#import <Shared/AppContext.h>

#import <optional>

std::optional<AppContext> appContext{};

@interface EngineView : MTKView <MTKViewDelegate>

@end

@implementation EngineView

- (void)mtkView:(MTKView *)__unused view drawableSizeWillChange:(CGSize) size
{
    if (appContext) {
        appContext->DeviceUpdate().Finish();
        appContext->Device().FinishRenderingCurrentFrame();

        appContext->Device().UpdateSize(static_cast<size_t>(size.width), static_cast<size_t>(size.height));

        appContext->Device().StartRenderingCurrentFrame();
        appContext->DeviceUpdate().Start();
    }
}

- (void)drawInMTKView:(MTKView *)__unused view
{
    if (appContext) {
        appContext->DeviceUpdate().Finish();
        appContext->Device().FinishRenderingCurrentFrame();
        appContext->Device().StartRenderingCurrentFrame();
        appContext->DeviceUpdate().Start();
    }
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    NSTrackingArea* trackingArea = [
        [NSTrackingArea alloc]
        initWithRect:NSZeroRect
        options:NSTrackingActiveAlways | NSTrackingInVisibleRect | NSTrackingMouseMoved
        owner:self
        userInfo:nil
        ];
    [[self view] addTrackingArea:trackingArea];
}

- (void)uninitialize {
    appContext.reset();
}

- (void)refreshBabylon {
    [self uninitialize];

    EngineView* engineView = [[EngineView alloc] initWithFrame:[self view].frame device:nil];
    engineView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [[self view] addSubview:engineView];
    engineView.delegate = engineView;

    size_t width = static_cast<size_t>(engineView.drawableSize.width);
    size_t height = static_cast<size_t>(engineView.drawableSize.height);

    appContext.emplace(
        engineView,
        width,
        height,
        [](const char* message)
        {
            NSLog(@"%s", message);
        },
        [](Napi::Env env)
        {
            auto statusCallback = Napi::Function::New(env, [](const Napi::CallbackInfo& info) {
                if (info.Length() > 0)
                {
                    auto message = info[0].ToString().Utf8Value();
                    NSLog(@"[Playground] %s", message.c_str());
                }
            });
            env.Global().Set("__nativePlaygroundStatus", statusCallback);
        });

    NSArray* arguments = [[NSProcessInfo processInfo] arguments];
    if (arguments.count == 1)
    {
        appContext->ScriptLoader().LoadScript("app:///Scripts/webgpu_smoke.js");
        appContext->ScriptLoader().LoadScript("app:///Scripts/playground_runner.js");
    }
    else
    {
        for (NSUInteger i = 1; i < arguments.count; i++) {
            appContext->ScriptLoader().LoadScript([arguments[i] UTF8String]);
        }

        appContext->ScriptLoader().LoadScript("app:///Scripts/playground_runner.js");
    }
}

- (void)viewDidAppear {
    [super viewDidAppear];

    [self refreshBabylon];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

    [self uninitialize];
}

- (void)mouseMoved:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)mouseDown:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)mouseDragged:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)mouseUp:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)otherMouseDown:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)otherMouseDragged:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)otherMouseUp:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)rightMouseDown:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)rightMouseDragged:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)rightMouseUp:(NSEvent *)theEvent {
    (void)theEvent;
}

- (void)scrollWheel:(NSEvent *)theEvent {
    (void)theEvent;
}

- (IBAction)refresh:(id)__unused sender
{
    [self refreshBabylon];
}

@end

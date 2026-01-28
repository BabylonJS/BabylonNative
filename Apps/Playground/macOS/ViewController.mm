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

    // Required for mouseMoved events.
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
        });

    NSArray *arguments = [[NSProcessInfo processInfo] arguments];
    if (arguments.count == 1)
    {
        appContext->ScriptLoader().LoadScript("app:///Scripts/experience.js");
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

- (CGFloat)getScreenHeight {
    return [self view].frame.size.height;
}

- (void)mouseMoved:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseMove(eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)mouseDown:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)mouseDragged:(NSEvent *)theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseMove(eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)mouseUp:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)otherMouseDown:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseDown(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)otherMouseDragged:(NSEvent *)theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseMove(eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)otherMouseUp:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)rightMouseDown:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)rightMouseDragged:(NSEvent *)theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseMove(eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)rightMouseUp:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        auto invertedY = [self getScreenHeight] - eventLocation.y;
        CGFloat screenScale = [[NSScreen mainScreen] backingScaleFactor];
        appContext->Input()->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, eventLocation.x * screenScale, invertedY * screenScale);
    }
}

- (void)scrollWheel:(NSEvent *) theEvent {
    if (appContext && appContext->Input())
    {
        appContext->Input()->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, -theEvent.deltaY);
    }
}

- (IBAction)refresh:(id)__unused sender
{
    [self refreshBabylon];
}

@end

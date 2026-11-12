#import "ViewController.h"

#import <Babylon/Embedding/Apple/BabylonNativeEmbedding.h>
#import <Babylon/Embedding/Apple/BNRuntimeNative.h>

#import <MetalKit/MTKView.h>

#include <Shared/CommandLine.h>
#include <Shared/PlaygroundScripts.h>

#include <stdexcept>

@implementation ViewController
{
    BNRuntime* _runtime;
    BNView* _bnView;
    MTKView* _mtkView;
}

- (void)viewDidLoad {
    [super viewDidLoad];

    // Required for mouseMoved events to be delivered to the view.
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
    // Tear down View first (closes in-flight frame, unbinds the surface),
    // then Runtime (joins the JS thread).
    _bnView = nil;
    _runtime = nil;
    [_mtkView removeFromSuperview];
    _mtkView = nil;
}

- (void)refreshBabylon {
    [self uninitialize];

    NSArray* arguments = [[NSProcessInfo processInfo] arguments];
    std::vector<const char*> argv;
    for (NSString* argument in arguments)
    {
        argv.push_back(argument.UTF8String);
    }
    const auto options = CommandLine::Parse(static_cast<int>(argv.size()), argv.data());
    BNRuntimeOptions* runtimeOptions = [[BNRuntimeOptions alloc] init];
    runtimeOptions.enableDebugger = YES;
    runtimeOptions.enableDebugTrace = options.DebugTrace.value_or(true);
    _runtime = [[BNRuntime alloc] initWithOptions:runtimeOptions];

    auto* nativeRuntime = Babylon::Embedding::Apple::RuntimeFromBNRuntime(_runtime);
    if (nativeRuntime == nullptr)
    {
        throw std::runtime_error{"Failed to create the Playground runtime"};
    }
    auto& runtime = *nativeRuntime;
    Playground::Initialize(options);
    Playground::QueueOptions(runtime, options);
    Playground::LoadBootstrapScripts(runtime);

    if (options.Scripts.empty())
    {
        [_runtime loadScript:@"app:///Scripts/experience.js"];
    }
    else
    {
        for (const auto& script : options.Scripts)
        {
            [_runtime loadScript:@(script.c_str())];
        }
        [_runtime loadScript:@"app:///Scripts/playground_runner.js"];
    }

    _mtkView = [[MTKView alloc] initWithFrame:[self view].frame device:nil];
    _mtkView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [[self view] addSubview:_mtkView];

    // BNView attaches the runtime to the MTKView and installs a default
    // MTKViewDelegate that drives per-frame render. Resize is driven
    // separately from -viewDidLayout below.
    _bnView = [[BNView alloc] initWithRuntime:_runtime view:_mtkView];
}

- (void)viewDidAppear {
    [super viewDidAppear];

    [self refreshBabylon];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

    [self uninitialize];
}

- (void)viewDidLayout {
    [super viewDidLayout];

    // Babylon Native owns the drawable size (BNView sets
    // autoResizeDrawable = NO), so MTKView no longer reports size changes
    // via its delegate. Drive resize explicitly from layout, passing
    // logical points; BNView applies the device-pixel-ratio internally.
    if (_bnView != nil)
    {
        const CGSize size = _mtkView.bounds.size;
        if (size.width > 0 && size.height > 0)
        {
            [_bnView resizeWithWidth:static_cast<NSUInteger>(size.width)
                              height:static_cast<NSUInteger>(size.height)];
        }
    }
}

#pragma mark - Input forwarding

// AppKit reports event locations in window coordinates with a bottom-left
// origin; Babylon (CSS) expects top-left. Convert here and pass logical
// pixels through unchanged — BNView handles device-pixel-ratio scaling.
- (NSPoint)logicalPointFromEvent:(NSEvent*)event {
    NSPoint location = [event locationInWindow];
    CGFloat height = [self view].frame.size.height;
    return NSMakePoint(location.x, height - location.y);
}

- (void)mouseMoved:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)mouseDown:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseDown:BNViewMouseButtonLeft atX:p.x y:p.y];
}

- (void)mouseDragged:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)mouseUp:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseUp:BNViewMouseButtonLeft atX:p.x y:p.y];
}

- (void)otherMouseDown:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseDown:BNViewMouseButtonMiddle atX:p.x y:p.y];
}

- (void)otherMouseDragged:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)otherMouseUp:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseUp:BNViewMouseButtonMiddle atX:p.x y:p.y];
}

- (void)rightMouseDown:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseDown:BNViewMouseButtonRight atX:p.x y:p.y];
}

- (void)rightMouseDragged:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)rightMouseUp:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseUp:BNViewMouseButtonRight atX:p.x y:p.y];
}

- (void)scrollWheel:(NSEvent*)theEvent {
    // Negate so scroll-up matches Babylon's negative-delta convention.
    [_bnView mouseWheel:BNViewMouseWheelAxisY delta:-theEvent.deltaY];
}

- (IBAction)refresh:(id)__unused sender {
    [self refreshBabylon];
}

@end

#import "ViewController.h"

#import <Babylon/Integrations/Apple/BabylonNativeIntegrations.h>
#import "AppleShared/PlaygroundBootstrap.h"

#import <MetalKit/MTKView.h>

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

    BNRuntimeOptions* options = [[BNRuntimeOptions alloc] init];
    options.enableDebugger = YES;
    options.enableDebugTrace = YES;
    _runtime = [[BNRuntime alloc] initWithOptions:options];

    [PlaygroundBootstrap loadScripts:_runtime];

    NSArray* arguments = [[NSProcessInfo processInfo] arguments];
    if (arguments.count == 1)
    {
        [_runtime loadScript:@"app:///Scripts/experience.js"];
    }
    else
    {
        for (NSUInteger i = 1; i < arguments.count; i++)
        {
            [_runtime loadScript:arguments[i]];
        }
        [_runtime loadScript:@"app:///Scripts/playground_runner.js"];
    }

    _mtkView = [[MTKView alloc] initWithFrame:[self view].frame device:nil];
    _mtkView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [[self view] addSubview:_mtkView];

    // BNView attaches the runtime to the MTKView and installs a default
    // MTKViewDelegate that drives per-frame render + resize.
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
    [_bnView mouseDown:BNView.leftMouseButton atX:p.x y:p.y];
}

- (void)mouseDragged:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)mouseUp:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseUp:BNView.leftMouseButton atX:p.x y:p.y];
}

- (void)otherMouseDown:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseDown:BNView.middleMouseButton atX:p.x y:p.y];
}

- (void)otherMouseDragged:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)otherMouseUp:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseUp:BNView.middleMouseButton atX:p.x y:p.y];
}

- (void)rightMouseDown:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseDown:BNView.rightMouseButton atX:p.x y:p.y];
}

- (void)rightMouseDragged:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseMoveAtX:p.x y:p.y];
}

- (void)rightMouseUp:(NSEvent*)theEvent {
    NSPoint p = [self logicalPointFromEvent:theEvent];
    [_bnView mouseUp:BNView.rightMouseButton atX:p.x y:p.y];
}

- (void)scrollWheel:(NSEvent*)theEvent {
    // Negate so scroll-up matches Babylon's negative-delta convention.
    [_bnView mouseWheel:BNView.mouseWheelY delta:static_cast<NSInteger>(-theEvent.deltaY)];
}

- (IBAction)refresh:(id)__unused sender {
    [self refreshBabylon];
}

@end

#import "LibNativeBridge.h"
#import <Shared/AppContext.h>

@implementation LibNativeBridge {
  std::optional<AppContext> _appContext;
  CADisplayLink *_displayLink;
}

+ (instancetype)sharedInstance {
  static LibNativeBridge *sharedInstance = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedInstance = [[self alloc] init];
  });
  return sharedInstance;
}

- (bool)initializeWithWidth:(NSInteger)width height:(NSInteger)height {
    if (self.initialized) {
        return YES;
    }
  
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render)];
    [_displayLink addToRunLoop:NSRunLoop.mainRunLoop forMode:NSDefaultRunLoopMode];
    
    _appContext.emplace(
        self.metalLayer,
        static_cast<size_t>(width),
        static_cast<size_t>(height),
        [](const char* message) { NSLog(@"%s", message); });

    _appContext->ScriptLoader().LoadScript("app:///Scripts/experience.js");

    self.initialized = YES;
    return true;
}

- (void)drawableWillChangeSizeWithWidth:(NSInteger)width height:(NSInteger)height {
    if (_appContext) {
        _appContext->DeviceUpdate().Finish();
        _appContext->Device().FinishRenderingCurrentFrame();

        _appContext->Device().UpdateSize(static_cast<size_t>(width), static_cast<size_t>(height));

        _appContext->Device().StartRenderingCurrentFrame();
        _appContext->DeviceUpdate().Start();
    }
}

- (void)setTouchDown:(int)pointerId x:(int)inX y:(int)inY {
  if (_appContext && _appContext->Input()) {
    _appContext->Input()->TouchDown(static_cast<int>(pointerId), static_cast<int>(inX), static_cast<int>(inY));
  }
}

- (void)setTouchMove:(int)pointerId x:(int)inX y:(int)inY {
  if (_appContext && _appContext->Input()) {
    _appContext->Input()->TouchMove(static_cast<int>(pointerId), static_cast<int>(inX), static_cast<int>(inY));
  }
}

- (void)setTouchUp:(int)pointerId x:(int)inX y:(int)inY {
  if (_appContext && _appContext->Input()) {
    _appContext->Input()->TouchUp(static_cast<int>(pointerId), static_cast<int>(inX), static_cast<int>(inY));
  }
}

- (void)render {
    if (_appContext && self.initialized) {
        _appContext->DeviceUpdate().Finish();
        _appContext->Device().FinishRenderingCurrentFrame();
        _appContext->Device().StartRenderingCurrentFrame();
        _appContext->DeviceUpdate().Start();
    }
}

- (void)shutdown {
    if (!self.initialized) {
        return;
    }

    _appContext.reset();

    [_displayLink invalidate];
    _displayLink = NULL;
    self.initialized = NO;
}

- (void)dealloc {
    [self shutdown];
}

@end

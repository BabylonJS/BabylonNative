#import <Foundation/Foundation.h>
#import <CompositorServices/CompositorServices.h>

@class CAMetalLayer;

@interface LibNativeBridge : NSObject

@property (nonatomic, assign, getter=isInitialized) BOOL initialized;
@property (nonatomic, strong) CAMetalLayer *metalLayer;

- (instancetype)initWithMetalLayer:(CAMetalLayer *)metalLayer;

- (void)setTouchMoveWithPointerId:(NSInteger)pointerId x:(NSInteger)x y:(NSInteger)y;
- (void)setTouchUpWithPointerId:(NSInteger)pointerId x:(NSInteger)x y:(NSInteger)y;
- (void)setTouchDownWithPointerId:(NSInteger)pointerId x:(NSInteger)x y:(NSInteger)y;
- (void)drawableWillChangeSizeWithWidth:(NSInteger)width height:(NSInteger)height;

- (BOOL)initializeWithWidth:(NSInteger)width height:(NSInteger)height;
- (void)shutdown;
- (void)render;

@end


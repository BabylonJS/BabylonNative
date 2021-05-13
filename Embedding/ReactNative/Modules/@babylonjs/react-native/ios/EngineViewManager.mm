#import "BabylonNativeInterop.h"

#import <React/RCTViewManager.h>
#import <React/RCTUIManager.h>
#import <ReactCommon/CallInvoker.h>

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <MetalKit/MetalKit.h>

@interface EngineView : MTKView

@property (nonatomic, copy) RCTDirectEventBlock onSnapshotDataReturned;

@end

@implementation EngineView {
    const RCTBridge* bridge;
    MTKView* xrView;
}

- (instancetype)init:(RCTBridge*)_bridge {
    if (self = [super initWithFrame:CGRectZero device:MTLCreateSystemDefaultDevice()]) {
        bridge = _bridge;

        super.translatesAutoresizingMaskIntoConstraints = false;
        super.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
        super.depthStencilPixelFormat = MTLPixelFormatDepth32Float;

        xrView = [[MTKView alloc] initWithFrame:self.bounds device:self.device];
        xrView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
        xrView.userInteractionEnabled = false;
        xrView.hidden = true;
        [self addSubview:xrView];
        [BabylonNativeInterop updateXRView:xrView];
    }
    return self;
}

- (void)setBounds:(CGRect)bounds {
    [super setBounds:bounds];
    [BabylonNativeInterop updateView:self];
}

- (void)touchesBegan:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [BabylonNativeInterop reportTouchEvent:self touches:touches event:event];
}

- (void)touchesMoved:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [BabylonNativeInterop reportTouchEvent:self touches:touches event:event];
}

- (void)touchesEnded:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [BabylonNativeInterop reportTouchEvent:self touches:touches event:event];
}

- (void)touchesCancelled:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    [BabylonNativeInterop reportTouchEvent:self touches:touches event:event];
}

- (void)drawRect:(CGRect)rect {
    if ([BabylonNativeInterop isXRActive]) {
        xrView.hidden = false;
    } else {
        xrView.hidden = true;
    }

    [BabylonNativeInterop renderView];
}

- (void)takeSnapshot {
    // We must take the screenshot on the main thread otherwise we might fail to get a valid handle on the view's image.
    dispatch_async(dispatch_get_main_queue(), ^{
        // Start the graphics context.
        UIGraphicsBeginImageContextWithOptions(self.bounds.size, YES /* opaque */, 0.0f);
        
        // Draw the current state of the view into the graphics context.
        [self drawViewHierarchyInRect:self.bounds afterScreenUpdates:NO];
        
        // Grab the image from the graphics context, and convert into a base64 encoded JPG.
        UIImage* capturedImage = UIGraphicsGetImageFromCurrentImageContext();
        UIGraphicsEndImageContext();
        NSData* jpgData = UIImageJPEGRepresentation(capturedImage, .8f);
        NSString* encodedData = [jpgData base64EncodedStringWithOptions:0];
        
        // Fire the onSnapshotDataReturned event if hooked up.
        if (self.onSnapshotDataReturned != nil) {
            self.onSnapshotDataReturned(@{ @"data":encodedData});
        }
    });
}

@end


@interface EngineViewManager : RCTViewManager
@end

@implementation EngineViewManager

RCT_EXPORT_MODULE(EngineViewManager)

RCT_EXPORT_VIEW_PROPERTY(onSnapshotDataReturned, RCTDirectEventBlock)

RCT_EXPORT_METHOD(takeSnapshot:(nonnull NSNumber*) reactTag) {
    // Marshal the takeSnapshot call to the appropriate EngineView.
    [self.bridge.uiManager addUIBlock:^(RCTUIManager *uiManager, NSDictionary<NSNumber*,UIView*>* viewRegistry) {
        EngineView* view = (EngineView*)viewRegistry[reactTag];
        if (!view || ![view isKindOfClass:[EngineView class]]) {
            return;
        }
        [view takeSnapshot];
    }];
}

- (UIView*)view {
    return [[EngineView alloc] init:self.bridge];
}

@end

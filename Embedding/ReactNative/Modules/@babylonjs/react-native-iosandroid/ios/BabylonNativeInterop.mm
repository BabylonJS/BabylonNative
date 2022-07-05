#import "BabylonNativeInterop.h"
#import "../../react-native/shared/BabylonNative.h"

#import <React/RCTBridge+Private.h>
#import <jsi/jsi.h>
#include <ReactCommon/CallInvoker.h>

#import <Foundation/Foundation.h>

#import <memory>

using namespace facebook;

@interface RCTBridge (RCTTurboModule)
- (std::shared_ptr<facebook::react::CallInvoker>)jsCallInvoker;
@end

namespace {
    jsi::Runtime* GetJSIRuntime(RCTBridge* bridge) {
        RCTCxxBridge* cxxBridge = reinterpret_cast<RCTCxxBridge*>(bridge);
        return reinterpret_cast<jsi::Runtime*>(cxxBridge.runtime);
    }
}

@implementation BabylonNativeInterop

static NSMutableArray* activeTouches = [NSMutableArray new];

+ (void)initialize:(RCTBridge*)bridge {
    auto jsCallInvoker{ bridge.jsCallInvoker };
    auto jsDispatcher{ [jsCallInvoker{ std::move(jsCallInvoker) }](std::function<void()> func)
    {
        jsCallInvoker->invokeAsync([func{ std::move(func) }]
        {
            func();
        });
    } };

    BabylonNative::Initialize(*GetJSIRuntime(bridge), std::move(jsDispatcher));

    [[NSNotificationCenter defaultCenter] removeObserver:self
        name:RCTBridgeWillInvalidateModulesNotification
        object:bridge.parentBridge];

    [[NSNotificationCenter defaultCenter] addObserver:self
        selector:@selector(onBridgeWillInvalidate:)
        name:RCTBridgeWillInvalidateModulesNotification
        object:bridge.parentBridge];
}

// NOTE: This happens during dev mode reload, when the JS engine is being shutdown and restarted.
+ (void)onBridgeWillInvalidate:(NSNotification*)notification
{
    BabylonNative::Deinitialize();
}

+ (void)updateView:(MTKView*)mtkView {
    const CGFloat scale = mtkView.contentScaleFactor;
    const int width = static_cast<int>(mtkView.bounds.size.width * scale);
    const int height = static_cast<int>(mtkView.bounds.size.height * scale);
    if (width != 0 && height != 0) {
        BabylonNative::UpdateView(mtkView, width, height);
    }
}

+ (void)updateMSAA:(NSNumber*)value {
    BabylonNative::UpdateMSAA([value unsignedCharValue]);
}

+ (void)renderView {
    BabylonNative::RenderView();
}

+ (void)resetView {
    BabylonNative::ResetView();
}

+ (void)updateXRView:(MTKView*)mtkView {
    BabylonNative::UpdateXRView(mtkView);
}

+ (bool)isXRActive {
    return BabylonNative::IsXRActive();
}

+ (void)reportTouchEvent:(MTKView*)mtkView touches:(NSSet<UITouch*>*)touches event:(UIEvent*)event {
    for (UITouch* touch in touches) {
        if (touch.view == mtkView) {
            const CGFloat scale = mtkView.contentScaleFactor;
            const CGPoint pointerPosition = [touch locationInView:mtkView];
            const uint32_t x = static_cast<uint32_t>(pointerPosition.x * scale);
            const uint32_t y = static_cast<uint32_t>(pointerPosition.y * scale);

            switch (touch.phase) {
                case UITouchPhaseBegan: {
                    // The activeTouches array only grows, it does not shrink (to keep indices constant since they are used as pointer ids),
                    // so look for an unused (null) array element and reuse it if found. Otherwise, add a new entry to the array.
                    NSUInteger pointerId = [activeTouches indexOfObject:[NSNull null]];
                    if (pointerId != NSNotFound) {
                        [activeTouches replaceObjectAtIndex:pointerId withObject:touch];
                    } else {
                        pointerId = [activeTouches count];
                        [activeTouches addObject:touch];
                    }
                    BabylonNative::SetTouchButtonState(static_cast<uint32_t>(pointerId), true, x, y);
                    break;
                }

                case UITouchPhaseMoved: {
                    NSUInteger pointerId = [activeTouches indexOfObject:touch];
                    if (pointerId != NSNotFound) {
                        BabylonNative::SetTouchPosition(static_cast<uint32_t>(pointerId), x, y);
                    }
                    break;
                }

                case UITouchPhaseEnded:
                case UITouchPhaseCancelled: {
                    NSUInteger pointerId = [activeTouches indexOfObject:touch];
                    if (pointerId != NSNotFound) {
                        [activeTouches replaceObjectAtIndex:pointerId withObject:[NSNull null]];
                        BabylonNative::SetTouchButtonState(static_cast<uint32_t>(pointerId), false, x, y);
                    }
                    break;
                }

                default:
                    break;
            }
        }
    }
}

@end

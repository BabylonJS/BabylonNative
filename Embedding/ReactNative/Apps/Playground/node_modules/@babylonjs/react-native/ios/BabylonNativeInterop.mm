#import "BabylonNativeInterop.h"
#import "BabylonNative.h"

#import <React/RCTBridge+Private.h>
#import <jsi/jsi.h>

#import <Foundation/Foundation.h>

#import <functional>
#import <memory>
#import <vector>
#import <unordered_map>
#import <mutex>

using namespace facebook;

namespace {
    jsi::Runtime* GetJSIRuntime(RCTBridge* bridge) {
        RCTCxxBridge* cxxBridge = reinterpret_cast<RCTCxxBridge*>(bridge);
        return reinterpret_cast<jsi::Runtime*>(cxxBridge.runtime);
    }
}

@implementation BabylonNativeInterop

static RCTBridge* currentBridge;
static MTKView* currentView;
static std::unique_ptr<Babylon::Native> currentNativeInstance;
static std::unordered_map<void*, std::vector<RCTPromiseResolveBlock>> initializationPromises;
static std::mutex mapMutex;
static NSMutableArray* activeTouches;

+ (void)setView:(RCTBridge*)bridge jsRunLoop:(NSRunLoop*)jsRunLoop mktView:(MTKView*)mtkView {
    const int width = static_cast<int>(mtkView.bounds.size.width * UIScreen.mainScreen.scale);
    const int height = static_cast<int>(mtkView.bounds.size.height * UIScreen.mainScreen.scale);
    if (width != 0 && height != 0) {
        // NOTE: jsRunLoop should only be null when remote debugging is enabled.
        //       In this case, we can just use the main loop, because we are only
        //       going to set an error state (which can happen on any thread).
        if (!jsRunLoop) {
            jsRunLoop = NSRunLoop.mainRunLoop;
        }

        [jsRunLoop performBlock:^{
            if (bridge != currentBridge) {
                currentBridge = bridge;
                [BabylonNativeInterop setCurrentNativeInstance:mtkView width:width height:height];
            } else if (currentNativeInstance) {
                if (mtkView != currentView) {
                    [BabylonNativeInterop setCurrentView:mtkView];
                    currentNativeInstance->Refresh((__bridge void*)currentView, width, height);
                } else {
                    // TODO: Figure out why resizing causes the app to crash
                    //currentNativeInstance->Resize(width, height);
                }
            }
        }];
    }
}

+ (void)reportTouchEvent:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event {
    if (currentNativeInstance) {
        for (UITouch* touch in touches) {
            if (touch.view == currentView) {
                const CGFloat scale = UIScreen.mainScreen.scale;
                const CGPoint pointerPosition = [touch locationInView:currentView];
                const uint32_t x = static_cast<uint32_t>(pointerPosition.x * scale);
                const uint32_t y = static_cast<uint32_t>(pointerPosition.y * scale);

                switch (touch.phase) {
                    case UITouchPhaseBegan: {
                        NSUInteger pointerId = [activeTouches indexOfObject:[NSNull null]];
                        if (pointerId == NSNotFound) {
                            pointerId = [activeTouches count];
                            [activeTouches addObject:touch];
                        } else {
                            [activeTouches replaceObjectAtIndex:pointerId withObject:touch];
                        }
                        currentNativeInstance->SetPointerButtonState(static_cast<uint32_t>(pointerId), 0, true, x, y);
                        break;
                    }

                    case UITouchPhaseMoved: {
                        NSUInteger pointerId = [activeTouches indexOfObject:touch];
                        currentNativeInstance->SetPointerPosition(static_cast<uint32_t>(pointerId), x, y);
                        break;
                    }

                    case UITouchPhaseEnded:
                    case UITouchPhaseCancelled: {
                        NSUInteger pointerId = [activeTouches indexOfObject:touch];
                        [activeTouches replaceObjectAtIndex:pointerId withObject:[NSNull null]];
                        currentNativeInstance->SetPointerButtonState(static_cast<uint32_t>(pointerId), 0, false, x, y);
                        break;
                    }

                    default:
                        break;
                }
            }
        }
    }
}

+ (void)whenInitialized:(RCTBridge*)bridge resolve:(RCTPromiseResolveBlock)resolve {
    if (bridge == currentBridge) {
        resolve([NSNumber numberWithUnsignedLong:reinterpret_cast<uintptr_t>(currentNativeInstance.get())]);
    } else {
        initializationPromises[(__bridge void*)bridge].push_back(resolve);
    }
}

+ (void)setCurrentView:(MTKView*)mtkView {
    currentView = mtkView;
    activeTouches = [NSMutableArray new];
}

+ (void)setCurrentNativeInstance:(MTKView*)mtkView width:(int)width height:(int)height {
    [BabylonNativeInterop setCurrentView:mtkView];

    const std::lock_guard<std::mutex> lock(mapMutex);

    currentNativeInstance.reset();

    jsi::Runtime* jsiRuntime = GetJSIRuntime(currentBridge);
    if (jsiRuntime) {
        currentNativeInstance = std::make_unique<Babylon::Native>(GetJSIRuntime(currentBridge), (__bridge void*)mtkView, width, height);
    }

    auto initializationPromisesIterator = initializationPromises.find((__bridge void*)currentBridge);
    if (initializationPromisesIterator != initializationPromises.end()) {
        for (RCTPromiseResolveBlock resolve : initializationPromisesIterator->second) {
            resolve([NSNumber numberWithUnsignedLong:reinterpret_cast<uintptr_t>(currentNativeInstance.get())]);
        }

        initializationPromises.erase(initializationPromisesIterator);
    }
}

@end

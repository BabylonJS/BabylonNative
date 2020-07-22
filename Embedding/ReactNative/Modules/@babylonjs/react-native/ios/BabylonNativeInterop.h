#pragma once

#import <MetalKit/MetalKit.h>
#import <React/RCTBridge.h>

@interface BabylonNativeInterop : NSObject
+ (void)setView:(RCTBridge*)bridge jsRunLoop:(NSRunLoop*)jsRunLoop mktView:(MTKView*)mtkView;
+ (void)reportTouchEvent:(NSSet<UITouch*>*)touches withEvent:(UIEvent*)event;
+ (void)whenInitialized:(RCTBridge*)bridge resolve:(RCTPromiseResolveBlock)resolve;
@end

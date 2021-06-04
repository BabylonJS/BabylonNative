#pragma once

#include <Foundation/Foundation.h>
#include <MetalKit/MetalKit.h>


@interface LibNativeBridge : NSObject

- (instancetype)init;
- (void)dealloc;

- (void)init:(MTKView*)inView width:(int)inWidth height:(int)inHeight xrView:(void*)xrView;
- (void)resize:(int)inWidth height:(int)inHeight;
- (void)render;
- (void)setInputs:(int)inX y:(int)inY tap:(bool)inTap;
- (bool)isXRActive;

@end


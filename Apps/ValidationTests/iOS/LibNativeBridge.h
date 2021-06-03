#pragma once

#include <Foundation/Foundation.h>
#include <MetalKit/MetalKit.h>


@interface LibNativeBridge : NSObject

- (instancetype)init;
- (void)dealloc;

- (void)init:(MTKView*)inView width:(int)inWidth height:(int)inHeight;
- (void)resize:(int)inWidth height:(int)inHeight;
- (void)render;

@end

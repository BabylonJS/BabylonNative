#import "BabylonNativeInterop.h"

#import <React/RCTBridgeModule.h>

#import <Foundation/Foundation.h>

@interface BabylonModule : NSObject <RCTBridgeModule>
@end

@implementation BabylonModule

RCT_EXPORT_MODULE();

@synthesize bridge = _bridge;

RCT_EXPORT_METHOD(initialize:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject) {
    [BabylonNativeInterop whenInitialized:self.bridge resolve:resolve];
}

RCT_EXPORT_METHOD(whenInitialized:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject) {
    [BabylonNativeInterop whenInitialized:self.bridge resolve:resolve];
}

@end

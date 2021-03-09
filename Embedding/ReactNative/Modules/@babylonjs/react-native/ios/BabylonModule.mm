#import "BabylonNativeInterop.h"

#import <React/RCTBridgeModule.h>
#import <ReactCommon/CallInvoker.h>

#import <Foundation/Foundation.h>

@interface RCTBridge (RCTTurboModule)
- (std::shared_ptr<facebook::react::CallInvoker>)jsCallInvoker;
@end

@interface BabylonModule : NSObject <RCTBridgeModule>
@end

@implementation BabylonModule

RCT_EXPORT_MODULE();

@synthesize bridge = _bridge;

RCT_EXPORT_METHOD(initialize:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject) {
    self.bridge.jsCallInvoker->invokeAsync([bridge{ self.bridge }, resolve]() {
        [BabylonNativeInterop initialize:bridge];
        resolve([NSNull null]);
    });
}

RCT_EXPORT_METHOD(resetView:(RCTPromiseResolveBlock)resolve reject:(RCTPromiseRejectBlock)reject) {
    dispatch_async(dispatch_get_main_queue(), ^{
        [BabylonNativeInterop resetView];
        resolve([NSNull null]);
    });
}

@end

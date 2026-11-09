// PlaygroundBootstrap.mm — calls into Apps/Playground/Shared/PlaygroundScripts.cpp
// so the bootstrap script list stays in one place.

#import "PlaygroundBootstrap.h"

#import <Babylon/Embedding/Apple/BNRuntime.h>

#include <Babylon/Embedding/Runtime.h>
#include <Shared/PlaygroundScripts.h>
#include <napi/napi.h>

#include <string>

// Re-declare the internal class extension that exposes the C++ Runtime*
// from BNRuntime (implementation lives in Embedding/Apple/Source/BNRuntime.mm).
@interface BNRuntime ()
- (Babylon::Embedding::Runtime*)nativeRuntime;
@end

@implementation PlaygroundBootstrap

+ (void)loadScripts:(BNRuntime*)runtime
{
    if (runtime == nil)
    {
        return;
    }
    Playground::Initialize();
    Playground::LoadBootstrapScripts(*[runtime nativeRuntime]);
}

+ (BOOL)loadMobileTestScripts:(BNRuntime*)runtime
{
#if BABYLON_NATIVE_PLAYGROUND_MOBILE_TESTS
    NSArray<NSString*>* arguments = NSProcessInfo.processInfo.arguments;
    NSUInteger index = [arguments indexOfObject:@"--mobile-test-url"];
    if (index == NSNotFound)
    {
        return NO;
    }
    if (index + 1 >= arguments.count)
    {
        @throw [NSException exceptionWithName:NSInvalidArgumentException
                                      reason:@"--mobile-test-url requires an HTTP URL."
                                    userInfo:nil];
    }
    NSString* endpoint = arguments[index + 1];
    NSURLComponents* url = [NSURLComponents componentsWithString:endpoint];
    if (url.host.length == 0 ||
        !([url.scheme isEqualToString:@"http"] || [url.scheme isEqualToString:@"https"]) ||
        url.user != nil || url.password != nil)
    {
        @throw [NSException exceptionWithName:NSInvalidArgumentException
                                      reason:@"--mobile-test-url requires an HTTP URL without credentials."
                                    userInfo:nil];
    }
    if (runtime == nil)
    {
        @throw [NSException exceptionWithName:NSInvalidArgumentException
                                      reason:@"Mobile visual tests require a runtime."
                                    userInfo:nil];
    }
    auto& nativeRuntime = *[runtime nativeRuntime];
    nativeRuntime.RunOnJsThread([endpoint = std::string{endpoint.UTF8String}](Napi::Env env) {
        env.Global().Set("__mobilePlaywrightUrl", Napi::String::New(env, endpoint));
    }, true);
    nativeRuntime.LoadScript("app:///Scripts/mobilePlaywright.js");
    return YES;
#else
    (void)runtime;
    return NO;
#endif
}

@end

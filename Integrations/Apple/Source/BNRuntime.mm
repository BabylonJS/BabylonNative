// BNRuntime.mm — Obj-C++ implementation bridging BNRuntime to
// Babylon::Integrations::Runtime.

#import "BNRuntimeInternal.h"
#import <Babylon/Integrations/Apple/BNRuntimeNative.h>

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

#include <memory>

@implementation BNRuntime
{
    std::unique_ptr<Babylon::Integrations::Runtime> _runtime;
    MTKView* _xrView;
}

- (instancetype)init
{
    return [self initWithEnableDebugger:NO shaderCachePath:nil];
}

- (instancetype)initWithEnableDebugger:(BOOL)enableDebugger
{
    return [self initWithEnableDebugger:enableDebugger shaderCachePath:nil];
}

- (instancetype)initWithEnableDebugger:(BOOL)enableDebugger
                       shaderCachePath:(nullable NSString*)shaderCachePath
{
    if ((self = [super init]))
    {
        Babylon::Integrations::RuntimeOptions options{};
        options.enableDebugger = enableDebugger ? true : false;
        // Default log sink: route every level to NSLog. Hosts that need
        // their own routing should drop down to the C++ API.
        options.log = [](Babylon::Integrations::LogLevel /*level*/, std::string_view message) {
            NSLog(@"%.*s", static_cast<int>(message.size()), message.data());
        };
        if (shaderCachePath != nil)
        {
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            options.shaderCachePath = shaderCachePath.UTF8String;
#else
            // Caller explicitly asked for shader caching but the
            // plugin wasn't compiled in. Fail loudly rather than
            // silently dropping the cache on the floor (which would
            // be hard to diagnose at runtime).
            @throw [NSException
                exceptionWithName:@"BabylonNativePluginNotEnabledException"
                           reason:@"shaderCachePath was provided but BABYLON_NATIVE_PLUGIN_SHADERCACHE was not enabled at native build time."
                         userInfo:nil];
#endif
        }
        _runtime = Babylon::Integrations::Runtime::Create(std::move(options));
    }
    return self;
}

- (void)loadScript:(NSString*)url
{
    if (url == nil)
    {
        return;
    }
    _runtime->LoadScript(url.UTF8String);
}

- (void)eval:(NSString*)source sourceURL:(NSString*)sourceURL
{
    if (source == nil)
    {
        return;
    }
    const char* src = source.UTF8String;
    const char* url = sourceURL ? sourceURL.UTF8String : "";
    _runtime->Eval(src, url);
}

- (void)suspend
{
    _runtime->Suspend();
}

- (void)resume
{
    _runtime->Resume();
}

- (BOOL)isSuspended
{
    return _runtime->IsSuspended() ? YES : NO;
}

- (void)setXrView:(MTKView*)xrView
{
#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    _xrView = xrView;
    _runtime->SetXrWindow((__bridge void*)xrView);
#else
    (void)xrView;
    @throw [NSException
        exceptionWithName:@"BabylonNativePluginNotEnabledException"
                   reason:@"setXrView: was called but BABYLON_NATIVE_PLUGIN_NATIVEXR was not enabled at native build time."
                 userInfo:nil];
#endif
}

- (BOOL)isXRActive
{
#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    return _runtime->IsXrActive() ? YES : NO;
#else
    // State query: "no XR session is active" is the correct answer when
    // XR isn't compiled in, so this is intentionally a non-throwing path.
    return NO;
#endif
}

- (Babylon::Integrations::Runtime*)nativeRuntime
{
    return _runtime.get();
}

- (void)updateXrViewIfNeeded
{
#if BABYLON_NATIVE_PLUGIN_NATIVEXR
    if (_xrView != nil)
    {
        const BOOL shouldBeHidden = !_runtime->IsXrActive();
        if (_xrView.hidden != shouldBeHidden)
        {
            _xrView.hidden = shouldBeHidden;
        }
    }
#endif
}

@end

namespace Babylon::Integrations::Apple
{
    Runtime* RuntimeFromBNRuntime(BNRuntime* runtime)
    {
        return runtime == nil ? nullptr : [runtime nativeRuntime];
    }
}


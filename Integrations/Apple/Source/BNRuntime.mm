// BNRuntime.mm — Obj-C++ implementation bridging BNRuntime to
// Babylon::Integrations::Runtime.

#import "BNRuntimeInternal.h"
#import <Babylon/Integrations/Apple/BNRuntimeNative.h>

#import <Foundation/Foundation.h>
#import <MetalKit/MetalKit.h>

#include <os/log.h>

#include <cstdint>
#include <memory>

namespace
{
    uint8_t ToRuntimeMSAASamples(NSNumber* msaaSamples)
    {
        const long long value = msaaSamples.longLongValue;
        return value >= 0 && value <= UINT8_MAX ? static_cast<uint8_t>(value) : 0;
    }

    // Lazily-initialized process-wide logger used by the default log
    // sink. Subsystem matches the Babylon Native CFBundleIdentifier
    // convention so Console.app / `log stream` can filter it cleanly.
    os_log_t BabylonNativeLogger()
    {
        static os_log_t logger = os_log_create("com.babylonjs.babylonnative", "Runtime");
        return logger;
    }

    // Map LogLevel onto the closest os_log type. os_log has no "warn"
    // distinct from "default", so Warn folds into DEFAULT (matching
    // Apple's own console.warn → default mapping). DEBUG and INFO are
    // filtered out of release builds and Console.app by default, so
    // routing Verbose there keeps DebugTrace spam out of the way unless
    // a developer explicitly opts in (`log stream --level debug ...`).
    os_log_type_t ToOSLogType(Babylon::Integrations::LogLevel level)
    {
        switch (level)
        {
            case Babylon::Integrations::LogLevel::Verbose: return OS_LOG_TYPE_DEBUG;
            case Babylon::Integrations::LogLevel::Log:     return OS_LOG_TYPE_DEFAULT;
            case Babylon::Integrations::LogLevel::Warn:    return OS_LOG_TYPE_DEFAULT;
            case Babylon::Integrations::LogLevel::Error:   return OS_LOG_TYPE_ERROR;
            case Babylon::Integrations::LogLevel::Fatal:   return OS_LOG_TYPE_FAULT;
        }
        return OS_LOG_TYPE_DEFAULT;
    }
}

@implementation BNRuntimeOptions

@end

@implementation BNRuntime
{
    std::unique_ptr<Babylon::Integrations::Runtime> _runtime;
    MTKView* _xrView;
}

- (instancetype)init
{
    return [self initWithOptions:nil];
}

- (instancetype)initWithOptions:(nullable BNRuntimeOptions*)runtimeOptions
{
    if ((self = [super init]))
    {
        Babylon::Integrations::RuntimeOptions options{};
        if (runtimeOptions != nil)
        {
            if (runtimeOptions.msaaSamples != nil)
            {
                options.msaaSamples = ToRuntimeMSAASamples(runtimeOptions.msaaSamples);
            }
            options.enableDebugger = runtimeOptions.enableDebugger ? true : false;
            options.enableDebugTrace = runtimeOptions.enableDebugTrace ? true : false;
            options.waitForDebugger = runtimeOptions.waitForDebugger ? true : false;
        }
        // Default log sink: route through `os_log_with_type` so the
        // level survives all the way to Console.app / `log stream`.
        // Hosts that need their own routing should drop down to the
        // C++ API. `%{public}.*s` is required to print the message
        // payload; without `{public}` os_log redacts non-scalar
        // arguments as `<private>` in release builds.
        options.log = [](Babylon::Integrations::LogLevel level, std::string_view message) {
            os_log_with_type(BabylonNativeLogger(), ToOSLogType(level),
                             "%{public}.*s",
                             static_cast<int>(message.size()), message.data());
        };
        if (runtimeOptions.shaderCachePath != nil)
        {
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            options.shaderCachePath = runtimeOptions.shaderCachePath.UTF8String;
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


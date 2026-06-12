// BNRuntime.mm — Obj-C++ implementation bridging BNRuntime to
// Babylon::Embedding::Runtime.

#import "BNRuntimeInternal.h"
#import <Babylon/Embedding/Apple/BNRuntimeNative.h>

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

    // Process-wide os_log channel for the default log sink. Subsystem
    // matches the Babylon Native CFBundleIdentifier convention so
    // Console.app / `log stream` can filter it.
    os_log_t BabylonNativeLogger()
    {
        static os_log_t logger = os_log_create("com.babylonjs.babylonnative", "Runtime");
        return logger;
    }

    // Map LogLevel onto os_log types. os_log has no distinct "warn", so
    // Warn folds into DEFAULT (matches Apple's console.warn → default).
    // DEBUG/INFO are filtered out of release builds and Console.app by
    // default, so Verbose lands there to suppress DebugTrace noise.
    os_log_type_t ToOSLogType(Babylon::Embedding::LogLevel level)
    {
        switch (level)
        {
            case Babylon::Embedding::LogLevel::Verbose: return OS_LOG_TYPE_DEBUG;
            case Babylon::Embedding::LogLevel::Log:     return OS_LOG_TYPE_DEFAULT;
            case Babylon::Embedding::LogLevel::Warn:    return OS_LOG_TYPE_DEFAULT;
            case Babylon::Embedding::LogLevel::Error:   return OS_LOG_TYPE_ERROR;
            case Babylon::Embedding::LogLevel::Fatal:   return OS_LOG_TYPE_FAULT;
        }
        return OS_LOG_TYPE_DEFAULT;
    }
}

@implementation BNRuntimeOptions

@end

@implementation BNRuntime
{
    std::optional<Babylon::Embedding::Runtime> _runtime;
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
        Babylon::Embedding::RuntimeOptions options{};
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
        // Default log sink: route through os_log_with_type so the level
        // reaches Console.app / `log stream`. Hosts wanting custom routing
        // should use the C++ API. `%{public}.*s` is required — without
        // `{public}` os_log redacts the payload as `<private>` in release.
        options.log = [](Babylon::Embedding::LogLevel level, std::string_view message) {
            os_log_with_type(BabylonNativeLogger(), ToOSLogType(level),
                             "%{public}.*s",
                             static_cast<int>(message.size()), message.data());
        };
        if (runtimeOptions.shaderCachePath != nil)
        {
#if BABYLON_NATIVE_PLUGIN_SHADERCACHE
            options.shaderCachePath = runtimeOptions.shaderCachePath.UTF8String;
#else
            // Fail loudly: silently dropping a caller-supplied cache path
            // would be hard to diagnose later.
            @throw [NSException
                exceptionWithName:@"BabylonNativePluginNotEnabledException"
                           reason:@"shaderCachePath was provided but BABYLON_NATIVE_PLUGIN_SHADERCACHE was not enabled at native build time."
                         userInfo:nil];
#endif
        }
        _runtime.emplace(std::move(options));
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
    // Non-throwing: "no XR active" is the correct answer when XR is off.
    return NO;
#endif
}

- (Babylon::Embedding::Runtime*)nativeRuntime
{
    return _runtime ? &*_runtime : nullptr;
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

namespace Babylon::Embedding::Apple
{
    Runtime* RuntimeFromBNRuntime(BNRuntime* runtime)
    {
        return runtime == nil ? nullptr : [runtime nativeRuntime];
    }
}


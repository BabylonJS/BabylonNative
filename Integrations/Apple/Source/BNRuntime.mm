// BNRuntime.mm — Obj-C++ implementation bridging BNRuntime to
// Babylon::Integrations::Runtime.

#import "BNRuntimeInternal.h"

#include <memory>

@implementation BNRuntime
{
    std::unique_ptr<Babylon::Integrations::Runtime> _runtime;
}

- (instancetype)init
{
    if ((self = [super init]))
    {
        _runtime = Babylon::Integrations::Runtime::Create();
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

- (Babylon::Integrations::Runtime*)nativeRuntime
{
    return _runtime.get();
}

@end

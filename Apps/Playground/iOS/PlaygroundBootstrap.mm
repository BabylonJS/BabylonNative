// PlaygroundBootstrap.mm — calls into Apps/Playground/Shared/PlaygroundScripts.cpp
// so the bootstrap script list stays in one place.

#import "PlaygroundBootstrap.h"

#import <Babylon/Integrations/Apple/BNRuntime.h>

#include <Babylon/Integrations/Runtime.h>
#include <Shared/PlaygroundScripts.h>

// Re-declare the internal class extension that exposes the C++ Runtime*
// from BNRuntime (implementation lives in Integrations/Apple/Source/BNRuntime.mm).
@interface BNRuntime ()
- (Babylon::Integrations::Runtime*)nativeRuntime;
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

@end

// PlaygroundBootstrap.mm — calls into Apps/Playground/Shared/PlaygroundScripts.cpp
// so the bootstrap script list stays in one place.

#import "PlaygroundBootstrap.h"

#import <Babylon/Embedding/Apple/BNRuntime.h>

#include <Babylon/Embedding/Runtime.h>
#include <Shared/PlaygroundScripts.h>

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

@end

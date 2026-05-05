// PlaygroundBootstrap.mm — implementation. Calls into the shared C++
// `Apps/Playground/Shared/PlaygroundScripts.cpp` so the bootstrap
// script list lives in one place.

#import "PlaygroundBootstrap.h"

#import <BabylonNativeIntegrations/BNRuntime.h>

#include <Babylon/Integrations/Runtime.h>
#include <Shared/PlaygroundScripts.h>

// Re-declare the internal class extension that exposes the C++
// `Runtime*` from `BNRuntime`. The actual implementation lives in
// `Integrations/Apple/Source/BNRuntime.mm`; declaring the same class
// extension here just makes the selector visible to the Obj-C++
// compiler in this translation unit.
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

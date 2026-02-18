#import <Cocoa/Cocoa.h>
#import <Babylon/DebugTrace.h>
#import "AppDelegate.h"

int main(int argc, const char * argv[]) {
    Babylon::DebugTrace::EnableDebugTrace(true);
    Babylon::DebugTrace::SetTraceOutput([](const char* trace) { NSLog(@"%s", trace); });

    @autoreleasepool
    {
        (void)argc;
        (void)argv;
        NSApplication* app = [NSApplication sharedApplication];
        AppDelegate* delegate = [[AppDelegate alloc] init];
        app.delegate = delegate;
        [app setActivationPolicy:NSApplicationActivationPolicyRegular];
        [app run];
        return 0;
    }
}

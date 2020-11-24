#import "AppDelegate.h"

@interface AppDelegate ()

@end

int GetExitCode();

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)__unused aNotification {
    // Insert code here to initialize your application
}


- (void)applicationWillTerminate:(NSNotification *)__unused aNotification {
    // Insert code here to tear down your application
    exit(GetExitCode());
}


- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)__unused sender {
    return YES;
}

@end

#import "AppDelegate.h"
#import "ViewController.h"

@interface AppDelegate ()
@property(strong) NSWindow* window;
@end

@implementation AppDelegate

- (void)applicationDidFinishLaunching:(NSNotification *)__unused aNotification {
    constexpr CGFloat kInitialWidth = 1280.0;
    constexpr CGFloat kInitialHeight = 720.0;

    NSRect frame = NSMakeRect(0.0, 0.0, kInitialWidth, kInitialHeight);
    self.window = [[NSWindow alloc]
        initWithContentRect:frame
                  styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable)
                    backing:NSBackingStoreBuffered
                      defer:NO];
    self.window.title = @"BabylonNative Playground";
    self.window.contentViewController = [[ViewController alloc] initWithNibName:nil bundle:nil];
    [self.window center];
    [self.window makeKeyAndOrderFront:nil];
    [NSApp activateIgnoringOtherApps:YES];
}

- (void)applicationWillTerminate:(NSNotification *)__unused aNotification {
    // Insert code here to tear down your application
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)sender {
    (void)sender;
    return YES;
}

@end

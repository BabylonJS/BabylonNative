#import "ViewController.h"
/*
#import <Babylon/AppRuntime.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeWindow.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <Babylon/ScriptLoader.h>
#import <Shared/InputManager.h>

std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<InputManager::InputBuffer> inputBuffer{};
*/

#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)refreshBabylon {
    // reset
    auto device = MTLCreateSystemDefaultDevice();
    NSLog(@"Got %@", device);
    (void)device;
    exit(-1);
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    [self refreshBabylon];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void)viewDidLayout {
    [super viewDidLayout];
}

- (void)mouseDown:(NSEvent *)__unused theEvent {
}

- (void)mouseDragged:(NSEvent *)__unused theEvent {

}

- (void)mouseUp:(NSEvent *)__unused theEvent {

}

-(IBAction) refresh:(id)__unused sender
{
    [self refreshBabylon];
}

@end

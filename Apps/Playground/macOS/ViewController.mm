#import "ViewController.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics/Device.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeInput.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Console.h>
#import <Babylon/Plugins/NativeCamera.h>
#import <Babylon/Plugins/NativeOptimizations.h>
#import <Babylon/ScriptLoader.h>
#import <MetalKit/MetalKit.h>

#import <math.h>
#import <optional>

std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
std::optional<Babylon::AppRuntime> runtime{};
std::optional<Babylon::Polyfills::Canvas> nativeCanvas{};
Babylon::Plugins::NativeInput* nativeInput{};

@interface EngineView : MTKView <MTKViewDelegate>

@end

@implementation EngineView

- (void)mtkView:(MTKView *)__unused view drawableSizeWillChange:(CGSize) size
{
    if (device) {
        update->Finish();
        device->FinishRenderingCurrentFrame();

        device->UpdateSize(static_cast<size_t>(size.width), static_cast<size_t>(size.height));

        device->StartRenderingCurrentFrame();
        update->Start();
    }
}

- (void)drawInMTKView:(MTKView *)__unused view
{
    if (device) {
        update->Finish();
        device->FinishRenderingCurrentFrame();
        device->StartRenderingCurrentFrame();
        update->Start();
    }
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
}

- (void)uninitialize {
    if (device)
    {
        update->Finish();
        device->FinishRenderingCurrentFrame();
    }

    nativeInput = {};
    nativeCanvas.reset();
    runtime.reset();
    update.reset();
    device.reset();
}

- (void)refreshBabylon {
    [self uninitialize];

    // parse command line arguments
    NSArray *arguments = [[NSProcessInfo processInfo] arguments];
    arguments = [arguments subarrayWithRange:NSMakeRange(1, arguments.count - 1)];
    __block std::vector<std::string> scripts;
    scripts.reserve([arguments count]);
    [arguments enumerateObjectsUsingBlock:^(NSString * _Nonnull obj, NSUInteger /*idx*/, BOOL * _Nonnull /*stop*/) {
        scripts.push_back([obj UTF8String]);
    }];

    EngineView* engineView = [[EngineView alloc] initWithFrame:[self view].frame device:nil];
    engineView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [[self view] addSubview:engineView];
    engineView.delegate = engineView;

    NSScreen *mainScreen = [NSScreen mainScreen];
    CGFloat screenScale = mainScreen.backingScaleFactor;
    size_t width = [self view].frame.size.width * screenScale;
    size_t height = [self view].frame.size.height * screenScale;

    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = engineView;
    graphicsConfig.Width = width;
    graphicsConfig.Height = height;
    graphicsConfig.MSAASamples = 4;

    device.emplace(graphicsConfig);
    update.emplace(device->GetUpdate("update"));
    device->StartRenderingCurrentFrame();
    update->Start();

    runtime.emplace();

    runtime->Dispatch([](Napi::Env env)
    {
        device->AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            NSLog(@"%s", message);
        });

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Plugins::NativeCamera::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.LoadScript("app:///Scripts/ammo.js");
    loader.LoadScript("app:///Scripts/recast.js");
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/babylon.gui.js");

    if (scripts.empty())
    {
        loader.LoadScript("app:///Scripts/experience.js");
    }
    else
    {
        for (const auto& script : scripts)
        {
            loader.LoadScript(script);
        }

        loader.LoadScript("app:///Scripts/playground_runner.js");
    }
}

- (void)viewDidAppear {
    [super viewDidAppear];

    [self refreshBabylon];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

    [self uninitialize];
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (CGFloat)getScreenHeight {
    return [self view].frame.size.height;
}

- (void)mouseDown:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseDown(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, eventLocation.x, invertedY);
     }
 }

 - (void)mouseDragged:(NSEvent *)theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseMove(eventLocation.x, invertedY);
     }
 }

 - (void)mouseUp:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseUp(Babylon::Plugins::NativeInput::LEFT_MOUSE_BUTTON_ID, eventLocation.x, invertedY);
     }
}

- (void)otherMouseDown:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseDown(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, eventLocation.x, invertedY);
     }
 }

 - (void)otherMouseDragged:(NSEvent *)theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseMove(eventLocation.x, invertedY);
     }
 }

 - (void)otherMouseUp:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseUp(Babylon::Plugins::NativeInput::MIDDLE_MOUSE_BUTTON_ID, eventLocation.x, invertedY);
     }
}

- (void)rightMouseDown:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseDown(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, eventLocation.x, invertedY);
     }
 }

 - (void)rightMouseDragged:(NSEvent *)theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseMove(eventLocation.x, invertedY);
     }
 }

 - (void)rightMouseUp:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseUp(Babylon::Plugins::NativeInput::RIGHT_MOUSE_BUTTON_ID, eventLocation.x, invertedY);
     }
}

- (void)scrollWheel:(NSEvent *) theEvent {
    if (nativeInput)
    {
        nativeInput->MouseWheel(Babylon::Plugins::NativeInput::MOUSEWHEEL_Y_ID, -theEvent.deltaY);
    }
}

- (IBAction)refresh:(id)__unused sender
{
    [self refreshBabylon];
}

@end

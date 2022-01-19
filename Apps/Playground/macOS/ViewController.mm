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

std::unique_ptr<Babylon::Graphics::Device> device{};
std::unique_ptr<Babylon::Graphics::Device::Update> update{};
std::unique_ptr<Babylon::AppRuntime> runtime{};
Babylon::Plugins::NativeInput* nativeInput{};
std::unique_ptr<Babylon::Polyfills::Canvas> nativeCanvas{};

@interface EngineView : MTKView <MTKViewDelegate>

@end

@implementation EngineView

- (void)mtkView:(MTKView *)__unused view drawableSizeWillChange:(CGSize) size
{
    if (device) {
        device->UpdateSize(static_cast<size_t>(size.width), static_cast<size_t>(size.height));
    }
}

- (void)drawInMTKView:(MTKView *)__unused view
{
    if (device) {
        device->StartRenderingCurrentFrame();
        update->Start();
        update->Finish();
        device->FinishRenderingCurrentFrame();
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
        device->FinishRenderingCurrentFrame();
    }

    // Note: JS Context owns this memory for this so it's not actually a leak
    nativeInput = {};
    runtime.reset();
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
<<<<<<< HEAD
    Babylon::Graphics::WindowConfiguration graphicsConfig{};
    graphicsConfig.WindowPtr = engineView;
=======
    Babylon::WindowConfiguration graphicsConfig{};
    graphicsConfig.Window = engineView;
>>>>>>> master
    graphicsConfig.Width = width;
    graphicsConfig.Height = height;
    device = Babylon::Graphics::Device::Create(graphicsConfig);
    update = std::make_unique<Babylon::Graphics::Device::Update>(device->GetUpdate("update"));

    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([](Napi::Env env)
    {
        device->AddToJavaScript(env);

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        nativeCanvas = std::make_unique <Babylon::Polyfills::Canvas>(Babylon::Polyfills::Canvas::Initialize(env));
        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            NSLog(@"%s", message);
        });

        Babylon::Plugins::Camera::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.Eval("document = {}", "");
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
    NSScreen *mainScreen = [NSScreen mainScreen];
    CGFloat screenScale = mainScreen.backingScaleFactor;
    return [self view].frame.size.height * screenScale;
}

- (void)mouseDown:(NSEvent *) theEvent {
     if (nativeInput)
     {
         NSPoint eventLocation = [theEvent locationInWindow];
         auto invertedY = [self getScreenHeight] - eventLocation.y;
         nativeInput->MouseDown(theEvent.buttonNumber, eventLocation.x, invertedY);
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
         nativeInput->MouseUp(theEvent.buttonNumber, eventLocation.x, invertedY);
     }
}

- (IBAction)refresh:(id)__unused sender
{
    [self refreshBabylon];
}

@end

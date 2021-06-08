#import "ViewController.h"

#import <filesystem>

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/ScriptLoader.h>
#import <MetalKit/MetalKit.h>
#import <Shared/TestUtils.h>

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::AppRuntime> runtime{};

@interface EngineView : MTKView <MTKViewDelegate>

@end

@implementation EngineView

- (void)mtkView:(MTKView *)__unused view drawableSizeWillChange:(CGSize) size
{
    if (graphics) {
        graphics->UpdateSize(static_cast<size_t>(size.width), static_cast<size_t>(size.height));
    }
}

- (void)drawInMTKView:(MTKView *)__unused view
{
    if (graphics) {
        graphics->FinishRenderingCurrentFrame();
        graphics->StartRenderingCurrentFrame();
    }
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSScreen *mainScreen = [NSScreen mainScreen];
    CGFloat screenScale = mainScreen.backingScaleFactor;
    self.preferredContentSize = CGSizeMake(600/screenScale, 400/screenScale);
}

- (void)uninitialize {
    if (graphics) {
        graphics->FinishRenderingCurrentFrame();
    }

    runtime.reset();
    graphics.reset();
}

- (void)initialize {
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

    Babylon::WindowConfiguration graphicsConfig{};
    graphicsConfig.WindowPtr = engineView;
    graphicsConfig.Width = static_cast<size_t>(600);
    graphicsConfig.Height = static_cast<size_t>(400);
    graphics = Babylon::Graphics::CreateGraphics(graphicsConfig);
    graphics->StartRenderingCurrentFrame();

    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([engineView](Napi::Env env)
    {
        graphics->AddToJavaScript(env);

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Canvas::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::TestUtils::CreateInstance(env, engineView);
    });

    Babylon::ScriptLoader loader{ *runtime };
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/babylon.gui.js");
    loader.LoadScript("app:///Scripts/validation_native.js");
}

- (void)viewDidAppear {
    [super viewDidAppear];

    [self initialize];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

    [self uninitialize];
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

@end

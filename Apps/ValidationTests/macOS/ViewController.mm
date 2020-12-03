#import "ViewController.h"

#import <filesystem>

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
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
    if (graphics != nullptr) {
        graphics->UpdateSize(static_cast<size_t>(size.width), static_cast<size_t>(size.height));
    }
}

- (void)drawInMTKView:(MTKView *)__unused view
{
    if (graphics != nullptr) {
        graphics->RenderCurrentFrame();
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

- (void)refreshBabylon {
    // reset
    runtime.reset();
    graphics.reset();

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

    void* windowPtr = (__bridge void*)engineView;

    graphics = Babylon::Graphics::CreateGraphics(windowPtr, static_cast<size_t>(600), static_cast<size_t>(400));
    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([windowPtr](Napi::Env env)
    {
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::DOM::Initialize(env);

        graphics->AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env, false); // render on UI Thread

        Babylon::TestUtils::CreateInstance(env, windowPtr);
    });
    
    Babylon::ScriptLoader loader{ *runtime };
    loader.LoadScript("app:///babylon.max.js");
    loader.LoadScript("app:///babylon.glTF2FileLoader.js");
    loader.LoadScript("app:///babylonjs.materials.js");
    loader.LoadScript("app:///babylon.gui.js");
    loader.LoadScript("app:///draco_decoder_gltf.js");
    loader.LoadScript("app:///validation_native.js");
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    [self refreshBabylon];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

    runtime.reset();
    graphics.reset();
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

@end

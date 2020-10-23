#import "ViewController.h"

#import <filesystem>

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeWindow.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/ScriptLoader.h>
#import <Shared/TestUtils.h>

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::AppRuntime> runtime{};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
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
    
    // Initialize NativeWindow plugin
    NSSize size = [self view].frame.size;
    float width = size.width;
    float height = size.height;
    NSWindow* nativeWindow = [[self view] window];
    void* windowPtr = (__bridge void*)nativeWindow;

    graphics = Babylon::Graphics::InitializeFromWindow(windowPtr, width, height);
    runtime = std::make_unique<Babylon::AppRuntime>();

    runtime->Dispatch([windowPtr, width, height](Napi::Env env)
    {
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);
        Babylon::Polyfills::Canvas::Initialize(env);

        Babylon::Plugins::NativeWindow::Initialize(env, windowPtr, width, height);

        graphics->AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env);
        Babylon::TestUtils::CreateInstance(env, windowPtr);
    });
    
    Babylon::ScriptLoader loader{ *runtime };
    loader.Eval("document = {}", "");
    loader.LoadScript("app:///babylon.max.js");
    loader.LoadScript("app:///babylon.glTF2FileLoader.js");
    loader.LoadScript("app:///babylonjs.materials.js");
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

- (void)viewDidLayout {
    [super viewDidLayout];
    if (runtime)
    {
        NSSize size = [self view].frame.size;
        float width = size.width;
        float height = size.height;
        graphics->UpdateSize(width, height);
        runtime->Dispatch([width, height](Napi::Env env)
        {
            Babylon::Plugins::NativeWindow::UpdateSize(env, static_cast<size_t>(width), static_cast<size_t>(height));
        });
    }
}

@end

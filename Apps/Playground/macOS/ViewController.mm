#import "ViewController.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <Babylon/ScriptLoader.h>
#import <Shared/InputManager.h>
#import <MetalKit/MetalKit.h>

std::unique_ptr<Babylon::Graphics> graphics{};
std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<InputManager<Babylon::AppRuntime>::InputBuffer> inputBuffer{};

@interface EngineView : MTKView <MTKViewDelegate>

@end

@implementation EngineView

- (void)mtkView:(MTKView *)__unused view drawableSizeWillChange:(CGSize)__unused size
{
    
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
}

- (void)refreshBabylon {
    // reset
    inputBuffer.reset();
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
    
    
    EngineView* engineView = [[EngineView alloc] initWithFrame:[self view].frame device:nil];
    [[self view] addSubview:engineView];
    engineView.delegate = engineView;
        
        
    //NSWindow* nativeWindow = [[self view] window];
    void* windowPtr = (__bridge void*)engineView;

    graphics = Babylon::Graphics::CreateGraphics(windowPtr, static_cast<size_t>(width), static_cast<size_t>(height));
    runtime = std::make_unique<Babylon::AppRuntime>();
    inputBuffer = std::make_unique<InputManager<Babylon::AppRuntime>::InputBuffer>(*runtime);

    runtime->Dispatch([](Napi::Env env)
    {
        Babylon::Polyfills::Window::Initialize(env);
        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        graphics->AddToJavaScript(env);
        Babylon::Plugins::NativeEngine::Initialize(env, false); // render on UI Thread
        
        InputManager<Babylon::AppRuntime>::Initialize(env, *inputBuffer);
    });
    
    Babylon::ScriptLoader loader{ *runtime };
    loader.Eval("document = {}", "");
    loader.LoadScript("app:///ammo.js");
    loader.LoadScript("app:///recast.js");
    loader.LoadScript("app:///babylon.max.js");
    loader.LoadScript("app:///babylon.glTF2FileLoader.js");
    loader.LoadScript("app:///babylonjs.materials.js");
    
    if (scripts.empty())
    {
        loader.LoadScript("app:///experience.js");
    }
    else
    {
        for (const auto& script : scripts)
        {
            loader.LoadScript(script);
        }

        loader.LoadScript("app:///playground_runner.js");
    }
}

- (void)viewDidAppear {
    [super viewDidAppear];
    
    [self refreshBabylon];
}

- (void)viewDidDisappear {
    [super viewDidDisappear];

    inputBuffer.reset();
    runtime.reset();
    graphics.reset();
}

- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

- (void)viewDidLayout {
    [super viewDidLayout];
    if (graphics)
    {
        NSSize size = [self view].frame.size;
        float width = size.width;
        float height = size.height;
        graphics->UpdateSize(width, height);
    }
}

- (void)mouseDown:(NSEvent *)__unused theEvent {
    if (inputBuffer)
    {
        inputBuffer->SetPointerDown(true);
    }
}

- (void)mouseDragged:(NSEvent *)theEvent {
    if (inputBuffer)
    {
        NSPoint eventLocation = [theEvent locationInWindow];
        inputBuffer->SetPointerPosition(eventLocation.x, eventLocation.y);
    }
}

- (void)mouseUp:(NSEvent *)__unused theEvent {
    if (inputBuffer)
    {
        inputBuffer->SetPointerDown(false);
    }
}

- (IBAction) refresh:(id)__unused sender
{
    [self refreshBabylon];
}

@end

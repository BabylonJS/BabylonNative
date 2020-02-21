#import "ViewController.h"

#import <Babylon/AppRuntime.h>
#import <Babylon/Console.h>
#import <Babylon/NativeEngine.h>
#import <Babylon/NativeWindow.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/XMLHttpRequest.h>
#import <Shared/InputManager.h>

std::unique_ptr<Babylon::AppRuntime> runtime{};
std::unique_ptr<InputManager::InputBuffer> inputBuffer{};

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

}

- (void)viewDidAppear {
    [super viewDidAppear];

    NSBundle *main = [NSBundle mainBundle];
    NSURL * resourceUrl = [main resourceURL];
    std::string rootUrl = [[NSString stringWithFormat:@"file://%s", [resourceUrl fileSystemRepresentation]] UTF8String];
    
    // Create the AppRuntime
    runtime.reset();
    runtime = std::make_unique<Babylon::AppRuntime>(rootUrl.data());
    
    // Initialize console plugin
    runtime->Dispatch([](Napi::Env env)
    {
        Babylon::Console::CreateInstance(env, [](const char* message, auto)
        {
            NSLog(@"%s", message);
        });
    });
    
    // Initialize NativeWindow plugin
    NSSize size = [self view].frame.size;
    float width = size.width;
    float height = size.height;
    NSWindow* nativeWindow = [[self view] window];
    void* windowPtr = (__bridge void*)nativeWindow;
    runtime->Dispatch([windowPtr, width, height](Napi::Env env)
    {
        Babylon::NativeWindow::Initialize(env, windowPtr, width, height);
    });
    
    Babylon::InitializeNativeEngine(*runtime, windowPtr, width, height);
    
    runtime->Dispatch([&runtime = *runtime](Napi::Env env)
    {
        Babylon::XMLHttpRequest::Initialize(env, runtime.RootUrl.data());
    });

    inputBuffer = std::make_unique<InputManager::InputBuffer>(*runtime);
    InputManager::Initialize(*runtime, *inputBuffer);
    
    Babylon::ScriptLoader loader{ *runtime, runtime->RootUrl };
    loader.LoadScript("babylon.max.js");
    loader.LoadScript("babylon.glTF2FileLoader.js");
    loader.LoadScript("experience.js");
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
        runtime->Dispatch([width, height](Napi::Env env)
        {
            auto& window = Babylon::NativeWindow::GetFromJavaScript(env);
            window.Resize(static_cast<size_t>(width), static_cast<size_t>(height));
        });
    }
}

- (void)mouseDown:(NSEvent *)theEvent {
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

- (void)mouseUp:(NSEvent *)theEvent {
    if (inputBuffer)
    {
        inputBuffer->SetPointerDown(false);
    }
}

@end

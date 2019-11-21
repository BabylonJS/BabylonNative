#include "LibNativeBridge.h"
#import <Babylon/Console.h>
#import <Babylon/RuntimeApple.h>
#import <Shared/InputManager.h>

std::unique_ptr<Babylon::RuntimeApple> runtime{};
std::unique_ptr<InputManager::InputBuffer> inputBuffer{};

struct LogHandler;
using Console = Babylon::Console<LogHandler>;
struct LogHandler
{
    void Log(const char* message, Console::LogLevel) const
    {
        NSLog(@"%s", message);
    }
};
LogHandler logHandler{};

@implementation LibNativeBridge

- (instancetype)init
{
    self = [super init];
    return self;
}

- (void)dealloc
{
    
}

- (void)init:( void* )CALayerPtr width:(int)inWidth height:(int)inHeight
{
    NSBundle* main = [NSBundle mainBundle];
    NSURL* resourceUrl = [main resourceURL];
    runtime = std::make_unique<Babylon::RuntimeApple>(
        CALayerPtr, [[NSString stringWithFormat:@"file://%s", [resourceUrl fileSystemRepresentation]] UTF8String]);
    
    runtime->Dispatch([](Babylon::Env& env)
    {
        auto jsConsole = Console::Create(env, logHandler);
        env.Global().Set("console", jsConsole.Value());
    });
    
    inputBuffer = std::make_unique<InputManager::InputBuffer>(*runtime);
    InputManager::Initialize(*runtime, *inputBuffer);
    
    runtime->LoadScript("babylon.max.js");
    runtime->LoadScript("babylon.glTF2FileLoader.js");
    runtime->LoadScript("experience.js");
}

- (void)setInputs:(int)x y:(int)y tap:(bool)tap
{
    
}

@end


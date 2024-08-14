#import "LibNativeBridge.h"
#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics/Device.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeInput.h>
#import <Babylon/Plugins/NativeOptimizations.h>
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Console.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>

@implementation LibNativeBridge {
    std::optional<Babylon::Graphics::Device> _device;
    std::optional<Babylon::Graphics::DeviceUpdate> _update;
    std::optional<Babylon::AppRuntime> _runtime;
    std::optional<Babylon::Polyfills::Canvas> _nativeCanvas;
    Babylon::Plugins::NativeInput* _nativeInput;
    BOOL _isXrActive;
}

- (instancetype)initWithMetalLayer:(CAMetalLayer *)metalLayer {
    self = [super init];
    if (self) {
        _metalLayer = metalLayer;
        _initialized = NO;
        _nativeInput = nullptr;
        _isXrActive = NO;
    }
    return self;
}

- (BOOL)initializeWithWidth:(NSInteger)width height:(NSInteger)height {
    if (self.initialized) {
        return YES;
    }
    
    Babylon::Graphics::Configuration graphicsConfig{};
    graphicsConfig.Window = self.metalLayer;
    graphicsConfig.Width = static_cast<size_t>(width);
    graphicsConfig.Height = static_cast<size_t>(height);

    _device.emplace(graphicsConfig);
    _update.emplace(_device->GetUpdate("update"));

    _device->StartRenderingCurrentFrame();
    _update->Start();

    _runtime.emplace();

    _runtime->Dispatch([self](Napi::Env env) {
        self->_device->AddToJavaScript(env);

        Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
            NSLog(@"%s", message);
        });

        self->_nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

        Babylon::Polyfills::Window::Initialize(env);

        Babylon::Polyfills::XMLHttpRequest::Initialize(env);

        Babylon::Plugins::NativeEngine::Initialize(env);

        Babylon::Plugins::NativeOptimizations::Initialize(env);

        self->_nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
    });

    Babylon::ScriptLoader loader{ *_runtime };
    loader.LoadScript("app:///Scripts/ammo.js");
    loader.LoadScript("app:///Scripts/recast.js");
    loader.LoadScript("app:///Scripts/babylon.max.js");
    loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
    loader.LoadScript("app:///Scripts/babylonjs.materials.js");
    loader.LoadScript("app:///Scripts/babylon.gui.js");
    loader.LoadScript("app:///Scripts/experience.js");
    self.initialized = YES;
    
    return YES;
}

- (void)drawableWillChangeSizeWithWidth:(NSInteger)width height:(NSInteger)height {
    if (_device) {
        _update->Finish();
        _device->FinishRenderingCurrentFrame();

        _device->UpdateSize(static_cast<size_t>(width), static_cast<size_t>(height));

        _device->StartRenderingCurrentFrame();
        _update->Start();
    }
}

- (void)setTouchUpWithPointerId:(NSInteger)pointerId x:(NSInteger)x y:(NSInteger)y {
    if (_nativeInput != nullptr) {
        _nativeInput->TouchUp(static_cast<int>(pointerId), static_cast<int>(x), static_cast<int>(y));
    }
}

- (void)setTouchMoveWithPointerId:(NSInteger)pointerId x:(NSInteger)x y:(NSInteger)y {
    if (_nativeInput != nullptr) {
        _nativeInput->TouchMove(static_cast<int>(pointerId), static_cast<int>(x), static_cast<int>(y));
    }
}

- (void)setTouchDownWithPointerId:(NSInteger)pointerId x:(NSInteger)x y:(NSInteger)y {
    if (_nativeInput != nullptr) {
        _nativeInput->TouchDown(static_cast<int>(pointerId), static_cast<int>(x), static_cast<int>(y));
    }
}

- (void)render {
    if (_device && self.initialized) {
        _update->Finish();
        _device->FinishRenderingCurrentFrame();
        _device->StartRenderingCurrentFrame();
        _update->Start();
    }
}

- (void)shutdown {
    if (!self.initialized) {
        return;
    }
    
    if (_device) {
        _update->Finish();
        _device->FinishRenderingCurrentFrame();
    }

    _nativeInput = nullptr;
    _nativeCanvas.reset();
    _runtime.reset();
    _update.reset();
    _device.reset();
    self.initialized = NO;
}

- (void)dealloc {
    [self shutdown];
}

@end

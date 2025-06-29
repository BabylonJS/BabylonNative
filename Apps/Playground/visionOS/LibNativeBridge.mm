#import "LibNativeBridge.h"
#import <Babylon/AppRuntime.h>
#import <Babylon/Graphics/Device.h>
#import <Babylon/ScriptLoader.h>
#import <Babylon/Plugins/NativeEngine.h>
#import <Babylon/Plugins/NativeInput.h>
#import <Babylon/Plugins/NativeOptimizations.h>
// NativeXr temporarily disabled to fix crashes
#import <Babylon/Polyfills/Canvas.h>
#import <Babylon/Polyfills/Console.h>
#import <Babylon/Polyfills/Window.h>
#import <Babylon/Polyfills/XMLHttpRequest.h>
#import <CompositorServices/CompositorServices.h>
#import <ARKit/ARKit.h>

@implementation LibNativeBridge {
  std::optional<Babylon::Graphics::Device> _device;
  std::optional<Babylon::Graphics::DeviceUpdate> _update;
  std::optional<Babylon::AppRuntime> _runtime;
  std::optional<Babylon::Polyfills::Canvas> _nativeCanvas;
  Babylon::Plugins::NativeInput* _nativeInput;
  bool _isXrActive;
  bool _isImmersiveMode;
  CADisplayLink *_displayLink;
  cp_layer_renderer_t _layerRenderer;
  ar_data_providers_t _dataProviders;
  cp_frame_t _frame;
}

+ (instancetype)sharedInstance {
  static LibNativeBridge *sharedInstance = nil;
  static dispatch_once_t onceToken;
  dispatch_once(&onceToken, ^{
    sharedInstance = [[self alloc] init];
  });
  return sharedInstance;
}

- (bool)initializeWithWidth:(NSInteger)width height:(NSInteger)height {
    if (self.initialized) {
        return YES;
    }
  
    _displayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(render)];
    [_displayLink addToRunLoop:NSRunLoop.mainRunLoop forMode:NSDefaultRunLoopMode];
    
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

        // NativeXr temporarily disabled to fix crashes
        // Babylon::Plugins::NativeXr::Initialize(env);
     
        _nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
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
    
    return true;
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

- (void)setTouchDown:(int)pointerId x:(int)inX y:(int)inY {
  if (_nativeInput) {
    _nativeInput->TouchDown(static_cast<int>(pointerId), static_cast<int>(inX), static_cast<int>(inY));
  }
}

- (void)setTouchMove:(int)pointerId x:(int)inX y:(int)inY {
  if (_nativeInput) {
    _nativeInput->TouchMove(static_cast<int>(pointerId), static_cast<int>(inX), static_cast<int>(inY));
  }
}

- (void)setTouchUp:(int)pointerId x:(int)inX y:(int)inY {
  if (_nativeInput) {
    _nativeInput->TouchUp(static_cast<int>(pointerId), static_cast<int>(inX), static_cast<int>(inY));
  }
}

- (void)render {
    if (_device && self.initialized) {
        if (_isImmersiveMode && _layerRenderer) {
            // Immersive mode rendering - simplified for now
            // TODO: Implement proper CompositorServices integration
            _update->Finish();
            _device->FinishRenderingCurrentFrame();
            _device->StartRenderingCurrentFrame();
            _update->Start();
        } else {
            // Regular window mode rendering
            _update->Finish();
            _device->FinishRenderingCurrentFrame();
            _device->StartRenderingCurrentFrame();
            _update->Start();
        }
    }
}

- (bool)initializeImmersiveMode {
    if (_isImmersiveMode) {
        return YES;
    }
    
    _isImmersiveMode = true;
    
    NSLog(@"🚀 Entering immersive mode - triggering XR session");
    
    // Signal to Babylon that we're in immersive/XR mode
    if (_runtime) {
        _runtime->Dispatch([](Napi::Env env) {
            // Set a flag to trigger XR when the scene is ready
            // This works with the existing experience.js timing
            env.RunScript(R"(
                // Set global flag for immersive mode
                window.shouldEnterImmersiveMode = true;
                console.log('🚀 Immersive mode flag set - waiting for scene initialization');
                
                // If scene is already ready and XR experience exists, enter immediately
                if (typeof window.xrExperience !== 'undefined' && window.xrExperience) {
                    console.log('🎯 XR already initialized, entering immersive mode now');
                    window.xrExperience.baseExperience.enterXRAsync('immersive-vr', 'unbounded', window.xrExperience.renderTarget).then(() => {
                        console.log('🌌 Successfully entered VR mode - scene should now be visible');
                    }).catch((error) => {
                        console.error('❌ Error entering XR mode:', error);
                    });
                }
            )");
        });
    }
    
    return YES;
}

- (void)exitImmersiveMode {
    if (!_isImmersiveMode) {
        return;
    }
    
    _isImmersiveMode = false;
    
    NSLog(@"📱 Exiting immersive mode");
    
    // Signal to Babylon that we're exiting immersive mode
    if (_runtime) {
        _runtime->Dispatch([](Napi::Env env) {
            // Exit XR session
            env.RunScript(R"(
                if (typeof scene !== 'undefined' && scene.onXRSessionEnded) {
                    console.log('🚪 Exiting XR session...');
                    // The XR session will be automatically cleaned up
                } else {
                    console.log('🚪 No active XR session to exit');
                }
            )");
        });
    }
}

- (void)shutdown {
    if (!self.initialized) {
        return;
    }
    
    [self exitImmersiveMode];
    
    if (_device) {
        _update->Finish();
        _device->FinishRenderingCurrentFrame();
    }

    _nativeInput = nullptr;
    _nativeCanvas.reset();
    _runtime.reset();
    _update.reset();
    _device.reset();
    [_displayLink invalidate];
    _displayLink = NULL;
    self.initialized = NO;
}

- (void)dealloc {
    [self shutdown];
}

@end

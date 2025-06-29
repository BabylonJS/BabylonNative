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
    
    // Auto-trigger immersive space after initialization
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(3.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        NSLog(@"🚀 Auto-triggering immersive space transition");
        [[NSNotificationCenter defaultCenter] postNotificationName:@"TriggerImmersiveSpace" object:nil];
    });
    
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
    
    NSLog(@"🚀 Entering immersive mode - setting up 3D scene");
    
    // Update rendering for immersive space
    if (_device && self.metalLayer) {
        NSLog(@"📱 Configuring rendering for immersive space");
        
        // Update viewport for immersive space
        CGSize drawableSize = self.metalLayer.drawableSize;
        [self drawableWillChangeSizeWithWidth:drawableSize.width height:drawableSize.height];
        
        NSLog(@"✅ Immersive rendering configured with size: %fx%f", drawableSize.width, drawableSize.height);
    }
    
    // Signal to Babylon that we're in immersive mode (non-XR)
    if (_runtime) {
        _runtime->Dispatch([](Napi::Env env) {
            env.RunScript(R"(
                // Set global flag for immersive mode
                window.isInImmersiveMode = true;
                console.log('🚀 Immersive mode flag set');
                
                // Check if scene is ready
                console.log('Scene exists:', typeof window.scene !== 'undefined');
                console.log('CreateImmersiveSpatialScene exists:', typeof CreateImmersiveSpatialScene !== 'undefined');
                
                // If scene exists, switch to immersive scene
                if (typeof window.scene !== 'undefined' && window.scene) {
                    console.log('🎯 Scene exists, switching to immersive view');
                    console.log('Current meshes count:', window.scene.meshes.length);
                    
                    // Clear existing scene
                    while (window.scene.meshes.length > 0) {
                        window.scene.meshes[0].dispose();
                    }
                    
                    // Create the immersive scene content
                    if (typeof CreateImmersiveSpatialScene === 'function') {
                        CreateImmersiveSpatialScene(window.scene);
                        console.log('🌌 Immersive spatial scene created');
                        console.log('New meshes count:', window.scene.meshes.length);
                        
                        // Force a render
                        window.scene.render();
                    } else {
                        console.error('❌ CreateImmersiveSpatialScene function not found!');
                    }
                    
                    // Adjust camera for immersive viewing
                    if (window.scene.activeCamera) {
                        window.scene.activeCamera.position = new BABYLON.Vector3(0, 1.6, 0);
                        console.log('Camera position set:', window.scene.activeCamera.position);
                    }
                } else {
                    console.error('❌ Scene not yet initialized!');
                    // Try again after a delay
                    setTimeout(() => {
                        if (window.scene && typeof CreateImmersiveSpatialScene === 'function') {
                            console.log('🔄 Retrying immersive scene setup...');
                            // Clear and recreate
                            while (window.scene.meshes.length > 0) {
                                window.scene.meshes[0].dispose();
                            }
                            CreateImmersiveSpatialScene(window.scene);
                            if (window.scene.activeCamera) {
                                window.scene.activeCamera.position = new BABYLON.Vector3(0, 1.6, 0);
                            }
                            window.scene.render();
                        }
                    }, 1000);
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
            env.RunScript(R"(
                window.isInImmersiveMode = false;
                console.log('🚪 Exiting immersive mode');
                
                // Restore the regular scene
                if (typeof window.scene !== 'undefined' && window.scene) {
                    // Clear immersive scene
                    while (window.scene.meshes.length > 0) {
                        window.scene.meshes[0].dispose();
                    }
                    
                    // Recreate the normal scene
                    if (typeof CreateBoxScene === 'function') {
                        CreateBoxScene(window.scene);
                        console.log('📦 Regular scene restored');
                    }
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

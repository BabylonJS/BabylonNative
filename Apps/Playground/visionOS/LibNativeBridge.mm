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
    
    // Auto-trigger immersive space after longer initialization
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
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
        // Unified rendering approach for both modes
        _update->Finish();
        _device->FinishRenderingCurrentFrame();
        _device->StartRenderingCurrentFrame();
        _update->Start();
    }
}

- (bool)switchToImmersiveLayer:(CAMetalLayer*)newLayer withWidth:(NSInteger)width height:(NSInteger)height {
    NSLog(@"🔄 Switching to immersive Metal layer: %@, size: %ldx%ld", newLayer, width, height);
    
    if (!newLayer) {
        NSLog(@"❌ Cannot switch - newLayer is nil");
        return NO;
    }
    
    if (!_device) {
        NSLog(@"❌ Cannot switch - _device is nil");
        return NO;
    }
    
    if (!_runtime) {
        NSLog(@"❌ Cannot switch - _runtime is nil");
        return NO;
    }
    
    NSLog(@"✅ All components valid, proceeding with switch");
    
    // Step 1: Update the metal layer property FIRST
    NSLog(@"🔧 Setting new metal layer");
    self.metalLayer = newLayer;
    
    // Step 2: Pause rendering temporarily
    NSLog(@"⏸️ Pausing current rendering");
    if (_displayLink) {
        [_displayLink setPaused:YES];
    }
    
    if (_update) {
        _update->Finish();
    }
    if (_device) {
        _device->FinishRenderingCurrentFrame();
    }
    
    // Step 3: Try simpler approach - just restart rendering with new layer
    NSLog(@"🎬 Restarting rendering with new Metal layer");
    
    try {
        // Simply restart rendering - don't try to update size which might cause issues
        _device->StartRenderingCurrentFrame();
        _update->Start();
        
        NSLog(@"✅ Rendering restarted successfully");
        
        // Resume display link
        if (_displayLink) {
            [_displayLink setPaused:NO];
        }
        
        NSLog(@"🎯 Successfully switched to immersive Metal layer");
        return YES;
        
    } catch (const std::exception& e) {
        NSLog(@"❌ Failed to restart rendering: %s", e.what());
        // Resume display link even on failure
        if (_displayLink) {
            [_displayLink setPaused:NO];
        }
        return NO;
    }
}

- (bool)initializeImmersiveMode {
    NSLog(@"🚀 initializeImmersiveMode called, current _isImmersiveMode: %d", _isImmersiveMode);
    
    if (_isImmersiveMode) {
        NSLog(@"⚠️ Already in immersive mode, returning YES");
        return YES;
    }
    
    _isImmersiveMode = true;
    NSLog(@"🌌 Setting immersive mode flag to true");
    
    // Simple immersive mode flag - let JavaScript handle scene updates
    if (_runtime) {
        NSLog(@"📡 Dispatching immersive mode script to JavaScript runtime");
        _runtime->Dispatch([](Napi::Env env) {
            env.RunScript(R"(
                // Set global flag for immersive mode
                window.isInImmersiveMode = true;
                console.log('🚀 JavaScript: Immersive mode activated');
                
                // Ensure scene exists and is visible
                if (typeof window.scene !== 'undefined' && window.scene) {
                    console.log('🎯 JavaScript: Scene available for immersive mode');
                    console.log('JavaScript: Current meshes count:', window.scene.meshes.length);
                    
                    // Make sure camera is positioned properly for immersive viewing
                    if (window.scene.activeCamera) {
                        window.scene.activeCamera.position = new BABYLON.Vector3(0, 1.6, 0);
                        window.scene.activeCamera.setTarget(new BABYLON.Vector3(0, 0, -3));
                        console.log('✅ JavaScript: Camera configured for immersive space');
                    }
                    
                    // Force a render to ensure visibility
                    if (window.scene.render) {
                        window.scene.render();
                        console.log('🎬 JavaScript: Forced scene render');
                    }
                } else {
                    console.error('❌ JavaScript: Scene not available for immersive mode');
                }
            )");
        });
        NSLog(@"✅ JavaScript script dispatched successfully");
    } else {
        NSLog(@"❌ Runtime is null, cannot dispatch JavaScript");
    }
    
    NSLog(@"🎯 initializeImmersiveMode completed successfully");
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

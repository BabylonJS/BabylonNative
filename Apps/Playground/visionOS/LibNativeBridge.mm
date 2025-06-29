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
    NSLog(@"🎬 LibNativeBridge.initializeWithWidth called: %ldx%ld", width, height);
    if (self.initialized) {
        NSLog(@"⚠️ Already initialized, returning YES");
        return YES;
    }
    NSLog(@"🚀 Starting LibNativeBridge initialization...");
  
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
    
    NSLog(@"✅ LibNativeBridge initialization completed successfully!");
    
    // DISABLED: Auto-trigger immersive space to prevent BGFX shutdown
    // dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(5.0 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
    //     NSLog(@"🚀 Auto-triggering immersive space transition");
    //     [[NSNotificationCenter defaultCenter] postNotificationName:@"TriggerImmersiveSpace" object:nil];
    // });
    
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
    static int frameCount = 0;
    frameCount++;
    
    if (_device && self.initialized) {
        // Unified rendering approach for both modes
        _update->Finish();
        _device->FinishRenderingCurrentFrame();
        _device->StartRenderingCurrentFrame();
        _update->Start();
        
        // Log every 60 frames to track rendering
        if (frameCount % 60 == 0) {
            NSLog(@"🎨 Frame %d - Rendering to layer: %@ (immersive: %d)", 
                  frameCount, self.metalLayer, _isImmersiveMode);
        }
    } else {
        if (frameCount % 60 == 0) {
            NSLog(@"⚠️ Frame %d - Skipping render (device: %@, initialized: %d)", 
                  frameCount, _device ? @"YES" : @"NO", self.initialized);
        }
    }
}

- (bool)switchToImmersiveLayer:(CAMetalLayer*)newLayer withWidth:(NSInteger)width height:(NSInteger)height {
    NSLog(@"🔄 switchToImmersiveLayer called with layer: %@, size: %ldx%ld", newLayer, width, height);
    
    if (!_device || !_runtime) {
        NSLog(@"❌ Cannot switch to immersive layer - device or runtime not ready");
        return NO;
    }
    
    NSLog(@"🔧 Safely switching graphics device to immersive layer");
    
    // Update the metal layer reference FIRST
    self.metalLayer = newLayer;
    NSLog(@"✅ Updated metalLayer reference to immersive layer");
    
    NSLog(@"🔧 Switching graphics device to immersive layer on main thread");
    
    // Do graphics device operations on main thread to respect thread affinity
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    auto successPtr = std::make_shared<bool>(false);
    
    dispatch_async(dispatch_get_main_queue(), ^{
        try {
            NSLog(@"🔧 Recreating graphics device on main thread");
            
            // Stop current rendering on main thread
            self->_update->Finish();
            self->_device->FinishRenderingCurrentFrame();
            NSLog(@"✅ Stopped current rendering on main thread");
            
            // Reset device and update
            self->_update.reset();
            self->_device.reset();
            NSLog(@"✅ Reset graphics device and update");
            
            // Create new graphics configuration with immersive layer
            Babylon::Graphics::Configuration graphicsConfig{};
            graphicsConfig.Window = newLayer;
            graphicsConfig.Width = static_cast<size_t>(width);
            graphicsConfig.Height = static_cast<size_t>(height);
            NSLog(@"🔧 Created graphics config for immersive layer: %ldx%ld", width, height);
            
            // Recreate device with new layer on main thread
            self->_device.emplace(graphicsConfig);
            self->_update.emplace(self->_device->GetUpdate("update"));
            NSLog(@"✅ Recreated graphics device with immersive layer");
            
            // Add device to JavaScript runtime and update scene
            self->_runtime->Dispatch([self](Napi::Env env) {
                self->_device->AddToJavaScript(env);
                NSLog(@"✅ Added graphics device to JavaScript runtime");
                
                // Force scene to update its render target
                env.RunScript(R"(
                    if (window.scene && window.engine) {
                        console.log('🔧 JavaScript: Updating scene render target after device switch');
                        
                        // Stop and restart render loop to ensure new device is used
                        window.engine.stopRenderLoop();
                        window.engine.runRenderLoop(function() {
                            window.scene.render();
                        });
                        
                        console.log('✅ JavaScript: Render loop restarted with new device');
                    }
                )");
            });
            
            // Start rendering to immersive layer on main thread
            self->_device->StartRenderingCurrentFrame();
            self->_update->Start();
            NSLog(@"✅ Started rendering to immersive layer");
            
            // CRITICAL: Force a frame to ensure bgfx picks up the new layer
            self->_update->Finish();
            self->_device->FinishRenderingCurrentFrame();
            self->_device->StartRenderingCurrentFrame();
            self->_update->Start();
            NSLog(@"✅ Forced frame update after layer switch");
            
            *successPtr = true;
        } catch (const std::exception& e) {
            NSLog(@"❌ Error recreating graphics device: %s", e.what());
            *successPtr = false;
        }
        
        dispatch_semaphore_signal(semaphore);
    });
    
    // Wait for completion
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    NSLog(@"🔧 Graphics device recreation completed with success: %d", *successPtr);
    
    return *successPtr;
}

- (bool)initializeImmersiveMode {
    NSLog(@"🚀 initializeImmersiveMode called, current _isImmersiveMode: %d", _isImmersiveMode);
    
    if (_isImmersiveMode) {
        NSLog(@"⚠️ Already in immersive mode, returning YES");
        return YES;
    }
    
    if (!_device || !_runtime) {
        NSLog(@"❌ Cannot initialize immersive mode - device or runtime not ready");
        return NO;
    }
    
    _isImmersiveMode = true;
    NSLog(@"🌌 Setting immersive mode flag to true");
    
    // Use JavaScript XR API to properly enter immersive mode
    if (_runtime) {
        NSLog(@"📡 Calling JavaScript enterImmersiveMode function");
        _runtime->Dispatch([](Napi::Env env) {
            env.RunScript(R"(
                // Set global flag for immersive mode
                window.isInImmersiveMode = true;
                console.log('🚀 JavaScript: Native bridge requesting immersive mode');
                
                // Call the JavaScript function to enter immersive mode properly
                if (typeof window.enterImmersiveMode === 'function') {
                    console.log('✅ JavaScript: Calling enterImmersiveMode function');
                    window.enterImmersiveMode();
                } else {
                    console.error('❌ JavaScript: enterImmersiveMode function not available');
                }
            )");
        });
        NSLog(@"✅ JavaScript enterImmersiveMode call dispatched");
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

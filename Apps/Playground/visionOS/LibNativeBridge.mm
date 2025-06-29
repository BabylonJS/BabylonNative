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
    if (_device && self.initialized) {
        // Unified rendering approach for both modes
        _update->Finish();
        _device->FinishRenderingCurrentFrame();
        _device->StartRenderingCurrentFrame();
        _update->Start();
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
    
    // Do all graphics device operations on the JavaScript thread to avoid threading issues
    dispatch_semaphore_t semaphore = dispatch_semaphore_create(0);
    auto successPtr = std::make_shared<bool>(false);
    
    _runtime->Dispatch([self, newLayer, width, height, semaphore, successPtr](Napi::Env env) {
        try {
            NSLog(@"🔧 Recreating graphics device on JavaScript thread");
            
            // Stop current rendering
            self->_update->Finish();
            self->_device->FinishRenderingCurrentFrame();
            NSLog(@"✅ Stopped current rendering");
            
            // Reset on JavaScript thread to avoid threading issues
            self->_update.reset();
            self->_device.reset();
            NSLog(@"✅ Reset graphics device and update on JS thread");
            
            // Create new graphics configuration with immersive layer
            Babylon::Graphics::Configuration graphicsConfig{};
            graphicsConfig.Window = newLayer;
            graphicsConfig.Width = static_cast<size_t>(width);
            graphicsConfig.Height = static_cast<size_t>(height);
            NSLog(@"🔧 Created graphics config for immersive layer: %ldx%ld", width, height);
            
            // Recreate device with new layer
            self->_device.emplace(graphicsConfig);
            self->_update.emplace(self->_device->GetUpdate("update"));
            NSLog(@"✅ Recreated graphics device with immersive layer");
            
            // Add the graphics device to JavaScript runtime
            self->_device->AddToJavaScript(env);
            NSLog(@"✅ Added graphics device to JavaScript runtime");
            
            // Start rendering to immersive layer
            self->_device->StartRenderingCurrentFrame();
            self->_update->Start();
            NSLog(@"✅ Started rendering to immersive layer");
            
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
    
    // Simple immersive mode flag - let JavaScript handle scene updates
    if (_runtime) {
        NSLog(@"📡 Dispatching immersive mode script to JavaScript runtime");
        _runtime->Dispatch([](Napi::Env env) {
            env.RunScript(R"(
                // Set global flag for immersive mode
                window.isInImmersiveMode = true;
                console.log('🚀 JavaScript: Immersive mode activated');
                
                // Set visible background color for debugging
                if (window.scene) {
                    window.scene.clearColor = new BABYLON.Color3(0.2, 0.2, 0.8); // Blue background
                    console.log('🎨 JavaScript: Set blue scene background');
                }
                
                // Check if scene is available (should be exposed from experience.js)
                if (typeof window.scene !== 'undefined' && window.scene) {
                    console.log('🎯 JavaScript: Scene available for immersive mode');
                    console.log('JavaScript: Current meshes count:', window.scene.meshes.length);
                    
                    // Clear existing meshes and create immersive content
                    while (window.scene.meshes.length > 0) {
                        window.scene.meshes[0].dispose();
                    }
                    
                    // Create immersive spatial scene
                    if (typeof CreateImmersiveSpatialScene === 'function') {
                        CreateImmersiveSpatialScene(window.scene);
                        console.log('✅ JavaScript: Created immersive spatial scene');
                    } else {
                        // Fallback: create multiple bright test objects
                        console.log('🎨 JavaScript: Creating bright fallback immersive content');
                        
                        // Create bright red sphere
                        var sphere = BABYLON.Mesh.CreateSphere("immersiveSphere", 16, 2.0, window.scene);
                        sphere.position = new BABYLON.Vector3(0, 1, -4);
                        var sphereMaterial = new BABYLON.StandardMaterial("sphereMat", window.scene);
                        sphereMaterial.emissiveColor = new BABYLON.Color3(1, 0, 0); // Bright red
                        sphereMaterial.diffuseColor = new BABYLON.Color3(1, 0, 0);
                        sphere.material = sphereMaterial;
                        
                        // Create bright green box
                        var box = BABYLON.Mesh.CreateBox("immersiveBox", 2.0, window.scene);
                        box.position = new BABYLON.Vector3(-3, 0, -4);
                        var boxMaterial = new BABYLON.StandardMaterial("boxMat", window.scene);
                        boxMaterial.emissiveColor = new BABYLON.Color3(0, 1, 0); // Bright green
                        boxMaterial.diffuseColor = new BABYLON.Color3(0, 1, 0);
                        box.material = boxMaterial;
                        
                        // Create bright blue cylinder
                        var cylinder = BABYLON.Mesh.CreateCylinder("immersiveCylinder", 2, 1, 1, 8, 1, window.scene);
                        cylinder.position = new BABYLON.Vector3(3, 0, -4);
                        var cylinderMaterial = new BABYLON.StandardMaterial("cylinderMat", window.scene);
                        cylinderMaterial.emissiveColor = new BABYLON.Color3(0, 0, 1); // Bright blue
                        cylinderMaterial.diffuseColor = new BABYLON.Color3(0, 0, 1);
                        cylinder.material = cylinderMaterial;
                        
                        // Add bright lighting
                        var light = new BABYLON.HemisphericLight("immersiveLight", new BABYLON.Vector3(0, 1, 0), window.scene);
                        light.intensity = 2.0; // Very bright
                        light.diffuse = new BABYLON.Color3(1, 1, 1);
                        light.specular = new BABYLON.Color3(1, 1, 1);
                        
                        console.log('✅ JavaScript: Created 3 bright fallback objects (sphere, box, cylinder) with lighting');
                    }
                    
                    // Configure camera for immersive viewing
                    if (window.scene.activeCamera) {
                        window.scene.activeCamera.position = new BABYLON.Vector3(0, 1.6, 0);
                        window.scene.activeCamera.setTarget(new BABYLON.Vector3(0, 0, -4));
                        console.log('✅ JavaScript: Camera configured for immersive space');
                    }
                    
                    // Force multiple renders to ensure visibility
                    for (var i = 0; i < 5; i++) {
                        if (window.scene.render) {
                            window.scene.render();
                        }
                    }
                    console.log('🎬 JavaScript: Forced multiple scene renders');
                    
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

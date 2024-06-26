#import "LibNativeBridge.hpp"
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

std::optional<Babylon::Graphics::Device> device{};
std::optional<Babylon::Graphics::DeviceUpdate> update{};
std::optional<Babylon::AppRuntime> runtime{};
std::optional<Babylon::Polyfills::Canvas> nativeCanvas{};
Babylon::Plugins::NativeInput* nativeInput{};

bool LibNativeBridge::initialize() {
  if (m_initialized) {
    return true;
  }
  
  Babylon::Graphics::Configuration graphicsConfig{};
  graphicsConfig.Window = m_layerRenderer;
  // Pass in visionOS default widht and height.
  graphicsConfig.Width = static_cast<size_t>(2732);
  graphicsConfig.Height = static_cast<size_t>(2048);

  device.emplace(graphicsConfig);
  update.emplace(device->GetUpdate("update"));

  device->StartRenderingCurrentFrame();
  update->Start();

  runtime.emplace();

  runtime->Dispatch([](Napi::Env env)
  {
      device->AddToJavaScript(env);

      Babylon::Polyfills::Console::Initialize(env, [](const char* message, auto) {
          NSLog(@"%s", message);
      });

      nativeCanvas.emplace(Babylon::Polyfills::Canvas::Initialize(env));

      Babylon::Polyfills::Window::Initialize(env);

      Babylon::Polyfills::XMLHttpRequest::Initialize(env);

      Babylon::Plugins::NativeEngine::Initialize(env);

      Babylon::Plugins::NativeOptimizations::Initialize(env);

      nativeInput = &Babylon::Plugins::NativeInput::CreateForJavaScript(env);
  });

  Babylon::ScriptLoader loader{ *runtime };
  loader.LoadScript("app:///Scripts/ammo.js");
  loader.LoadScript("app:///Scripts/recast.js");
  loader.LoadScript("app:///Scripts/babylon.max.js");
  loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
  loader.LoadScript("app:///Scripts/babylonjs.materials.js");
  loader.LoadScript("app:///Scripts/babylon.gui.js");
  loader.LoadScript("app:///Scripts/experience.js");
  m_initialized = true;
  return true;
}

void LibNativeBridge::render() {
  if (device && m_initialized)
  {
      update->Finish();
      device->FinishRenderingCurrentFrame();
      device->StartRenderingCurrentFrame();
      update->Start();
  }
}

void LibNativeBridge::shutdown() {
  if (!m_initialized) {
    return;
  }
  
  if (device)
  {
      update->Finish();
      device->FinishRenderingCurrentFrame();
  }

  nativeInput = {};
  nativeCanvas.reset();
  runtime.reset();
  update.reset();
  device.reset();
  m_initialized = false;
}

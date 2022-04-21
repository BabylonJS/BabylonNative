# Using external textures

This page is intended to give a initial overview on how to provide Babylon Native with native graphics API textures.

## ExternalTexture plugin library

ExternalTexture is a plugin library that provided the functionality required to send a BabylonJS texture using a native graphics API texture object. By including this library, users will have access to the `Babylon::Plugin::ExternalTexture` that can be constructed from a native texture and used to generate JS objects.

## Properly Initialize `Babylon::Graphics::Device`

In order to be able to consume native textures, it is important that both Babylon Native and the consuming application are using the same graphics device. One way to achieve this is to create a `Babylon::Graphics::Device` using a `Babylon::Graphics::DeviceConfiguration` where the consuming application can provide the graphics device directly. The application can then set the window using `Babylon::Graphics::WindowConfiguration` by calling `Babylon::Graphics::Device::UpdateWindow`. See example below:

```cpp
// Create graphics Device
Babylon::Graphics::DeviceConfiguration config;
config.Device = d3dDevice;
config.DevicePixelRatio = InitializeDPIScale(hWnd);
Babylon::Graphics::Device device = Babylon::Graphics::Device::Create(config);

RECT rect;
if (!GetClientRect(hWnd, &rect))
{
    return;
}

auto width = static_cast<size_t>(rect.right - rect.left);
auto height = static_cast<size_t>(rect.bottom - rect.top);

Babylon::Graphics::WindowConfiguration winConfig;
winConfig.Window = hWnd;
winConfig.Width = width;
winConfig.Height = height;

device->UpdateWindow(winConfig);
```

## ExternalTexture class

`Babylon::Plugins::ExternalTexture` is a class that can be constructed with a native texture and then used to send to the JavaScript environment using `ExternalTexture::AddToContextAsync`. It is important to note that the constructor will only store the necessary information to convert the native texture to the bgfx texture, but it will _not_ create bgfx texture. This class will hold a strong reference to the native texture when possible. The native texture ownership will be shared with JS when `ExternalTexture::AddToContextAsync` is called. It is safe to destroy this class before `ExternalTexture::AddToContextAsync` async operation completes.

This class assumes that the native texture was created using the same graphics device used to create the Babylon::Device. See [Properly Initialize `Babylon::Graphics::Device`](#properly-initialize-babylongraphicsdevice).

The following will happen inside a call to `ExternalTexture::AddToContextAsync`:

- A `Napi::Promise` will be created to encapsulate the async operation over a frame.
- During `Babylon::Graphics::DeviceContext::BeforeRenderScheduler()`, a new dummy bgfx texture will be created.
- During `Babylon::Graphics::DeviceContext::AfterRenderScheduler()`, this bgfx texture will be overridden with the native texture.
- On the JS thread, a `Napi::Pointer` will be created to hold the texture and the JS promise will resolved with this object.

It is safe to create multiple JS objects from the same `Babylon::Plugins::ExternalTexture` via `ExternalTexture::AddToContextAsync`.

Once the JS texture is available on the JS side, use `engine.wrapNativeTexture` to create an Babylon.js `InternalTexture`.
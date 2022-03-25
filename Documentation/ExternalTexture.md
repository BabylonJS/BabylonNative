# Using external textures

This page is intended to give a initial overview on how to provide Babylon Native with native graphics API textures.

## ExternalTexture plugin library

ExternalTexture is a plugin library that provided the functionality required to send a BabylonJS texture using a native graphics API texture object. By including this library users will have access to the ```Babylon::Plugin::ExternalTexture``` that can be constructed from a native texture handler and used to generate JS objects.

## Properly initialize ```Babylon::Graphics::Device```

In order to be able to consume native textures it is important that both Babylon Native and the consuming application are using the same Graphics Device (D3D11Device equivalents). For this, ```Babylon::Graphics::Device``` must be created using a ```DeviceConfiguration``` were the consuming application can provide its graphics device as a field. The application then needs to set the drawing window that would be set when using ```WindowConfiguration```, it can do so by calling ```Babylon::Graphics::Device::UpdateWindow``` (see example below).

```cpp
// Create graphics Device
Babylon::Graphics::DeviceConfiguration config;
config.Device = d3dDevice;
config.DevicePixelRatio = InitializeDPIScale(hWnd);
Babylon::Graphics::Device device = Babylon::Graphics::Device::Create(config);

RECT rect;
if (!GetClientRect(mhMainWnd, &rect))
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

ExternalTexture is a class that can be constructed using a native graphics API handle and then be used to create Babylon::Graphics::TextureData and send this data to JavaScript environment using ExternalTexture::AddToContext(). It is important to notice that the constructor will only store the necessary information to convert the native texture to the bgfx internal texture, but it will _not_ create bgfx object.

This class also assumes that the native texture was created using the same graphics device used to create the Babylon::Device. Therefore, it is assumed that ```Babylon::Graphics::Device``` was created by providing a ```Babylon::Graphics::DeviceConfiguration``` structure with the appropriate Device.

The following things will happen once ExternalTexture::AddToContext() is called:

- Napi::Promise will be created, since the actual creation of the texture will happen over an entire frame. 
- During BeforeRender, a bgfx texture will be created.
- During AfterRender, this bgfx texture will be replaced to use the native provided texture instead. A TextureData will be created.
- In the JS thread a Napi::Pointer<TextureData> will be created and the Napi::Promise::Deferred will be resolved. (The bgfx texture will be released by the Napi::Pointer finalization callback)

With this approach the same native texture can be used to create multiple JS objects, each one will have ownership over the bgfx texture representation but not the actual native texture.
# Using external textures

This page is intended to give a initial overview on how to provide Babylon Native with native graphics API textures.

## ExternalTexture plugin library

ExternalTexture is a plugin library that provided the functionality required to send a BabylonJS texture using a native graphics API texture object. By including this library users will have access to the ```Babylon::Plugin::ExternalTexture``` that can be constructed from a native texture handler and used to generate JS objects.

## Properly initialize ```Babylon::Graphics::Device```

In order to be able to consume native textures it is important that both Babylon Native and the consuming application are using the same Graphics Device (D3D11Device equivalents). For this, ```Babylon::Graphics::Device``` must be created using a ```DeviceConfiguration``` were the consuming application can provite its graphics device as a field. If the application than needs to set the drawing window that would be set when using ```WindowConfiguration```, it can do so by calling ```Babylon::Graphics::Device::UpdateWindow``` (see example below).

```cpp
/// Create graphics Device
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

ExternalTexture is a class that can be constructed using a native graphics API handle and then be used to create Babylon::Graphics::TextureData and send this data to javascript environment using ExternalTexture::AddToContext(). It is important to notice that the constructor will only store the necessary to convert the native texture to the bgfx internal texture, but it will not create bgfx object. 

This class also assumes that the native texture was created using the same graphics device used to create the Babylon::Device. Therefore, it is assumed that ```Babylon::Graphics::Device``` was created by providing a ```Babylon::Graphics::DeviceConfiguration``` structure with the appropriate Device.

Only when ExternalTexture::AddToContext() is called that the following things will happen: 

-  Napi::Promise will be created, since the actual creation of the texture will happen over an entire frame. 
- During BeforeRender a bgfx texture will be created.
- During AfterRender this bgfx texture will be replaced to use the native provided texture instead. A TextureData will be created.
- In the JS thread a Napi::Pointer<TextureData> will be created and the Napi::Promise::Deferred will be resolved. (The bgfx texture will be released by the Napi::Pointer finalization callback)

With this approach the same native texture can be used to create multiple JS objects, each one will have ownership over the bgfx texture representation but not the actual native texture.

This class also provide methods for retreiving texture information such as width, height and format, those will be required by the JS se to create the texture object.

## Handling the result of ```ExternalTexture::AddToContext()```

It will be resposibility of the consuming code to handle the promise return by ```ExternalTexture::AddToContext```  and send it over to the javascript side to create a Babylon Texture object, the following code show and example on how to do it:

C++ code:

```cpp
///Called when JS is ready.
void BabylonJs_OnReady(const Napi::CallbackInfo& info)
{
    auto environment = info.Env();
    auto jsObject = externalTexture->AddToContext(environment);

    auto width = externalTexture->GetWight();
    auto height = externalTexture->GetHeight();
    auto format = externalTexture->GetFormat();

    // Tell the JS side about the texture.
    info.Env().Global().Get("loadNativeTexture").As<Napi::Function>().Call({
        // Wrap the texture handle into a JavaScript type that can be consumed by the native engine.
        // This transfers ownership of the texture to the Babylon ThinTexture on the js side.
        // Important: We cannot do this twice with the same native texture handle.
        jsObject,
        Napi::Value::From(info.Env(), width),
        Napi::Value::From(info.Env(), height),
        Napi::value::From(info.Env(), format)
    });
}
```


Receiving javascript code:

```javascript
function loadNativeTexture(nativePromise, width, height, format)
{
    nativePromise.then((nativeTexture) =>
    {
       const internalTexture = new BABYLON.InternalTexture(
            engine,
            BABYLON.InternalTextureSource.Unknown,
            true
        );
        internalTexture._hardwareTexture = new BABYLON.WebGLHardwareTexture(
            nativeTexture
        );

        internalTexture.generateMipMaps = false;
        internalTexture.baseWidth = internalTexture.width = width;
        internalTexture.baseHeight = internalTexture.height = height;
        internalTexture.format = format;
        internalTexture.isReady = true;
        inTexture = new BABYLON.ThinTexture(internalTexture);
});
```
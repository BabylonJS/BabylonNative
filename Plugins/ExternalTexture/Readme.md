# Using external textures

This page is intended to give a initial overview on how to provide Babylon Native with native graphics API textures.

## ExternalTexture plugin library

ExternalTexture is a plugin library that provided the functionality required to send a BabylonJS texture using a native graphics API texture object. By including this library, users will have access to the `Babylon::Plugin::ExternalTexture` that can be constructed from a native texture and used to generate JS objects.

## Properly Initialize `Babylon::Graphics::Device`

"To consume native textures, it's crucial that both Babylon Native and the consuming application use the same graphics device. One way to achieve this is by creating a ```Babylon::Graphics::Device``` using a ```Babylon::Graphics::Configuration```, where the consuming application can provide the graphics device directly.

```cpp
// Create graphics Device
Babylon::Graphics::Configuration graphicsConfig {};
graphicsConfig.Device = d3dDevice;
Babylon::Graphics::Device babylonDevice = Babylon::Graphics::Device(config);
```

If the external texture will be used as a render target, there is no need to provide a width, height or Window values for the Configuration object.

To set the ExternalTexture to be used as a render target by Babylon.js one must call a JS function that will assign the 

```
int width  // Your render target width.
int height // Your render target height. 

std::promise<void> textureCreationSubmitted {};
std::promise<void> textureCreationDone {};

jsRuntime.Dispatch([&externalTexture, &textureCreationSubmitted, width, height, &textureCreationDone](Napi::Env env)
{
   auto jsPromisse = externalTexture->AddToContextAsync(env);
   auto result = env.Global().Get("YOUR_JS_FUNCTION").As<Napi::Function>().Call(
      { 
         jsPromisse, 
         Napi::Value::From(env, width),
         Napi::Value::From(env, height),
         Napi::Function::New(env, [&textureCreationDone](const Napi::CallbackInfo& info)
         {
            textureCreationDone.set_value();
         })
      });
   textureCreationSubmitted.set_value();
});

// Wait for texture creation to be submitted.
textureCreationSubmitted.get_future().get();

// Run 1 render loop so the texture can get created. 
m_update->Finish();
m_device->FinishRenderingCurrentFrame();
m_device->StartRenderingCurrentFrame();
m_update->Start();

// Wait for callback to confirm the texture is created on the JS side.
textureCreated.get_future().get();
```

The usual JS function to assign the external texture object as a render target for the Babylon scene camera looks like the following:

```js
function YOUR_JS_FUNCTION(externalTexturePromisse, width, height, textureCreatedCallback) {
    externalTexturePromisse.then((externalTexture) => {
        const outputTexture = engine.wrapNativeTexture(externalTexture);
        scene.activeCamera.outputRenderTarget = new BABYLON.RenderTargetTexture(
            "ExternalTexture",
            {
                width: width,
                height: height,
            },
            scene,
            {
                colorAttachment: outputTexture,
                generateDepthBuffer: true,
                generateStencilBuffer: true,
            }
        );
        textureCreatedCallback();
    });
}
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

## Example 1: Copy rendering content (D3D12)

The following example shows how to read back the content of a frame buffer that was created using D3D12 and provided as a render target to Babylon Native using a External texture. The internal ```ID3D12CommandQueue``` used by Babylon Native can be retrived by calling ```ID3D12Device::GetPrivateData```. Once the hosting application has access to the command ```ID3D12CommandQueue``` it can create a new command list to copy the content of the frame buffer ```ID3D12Resource``` to CPU visible memory.

The code assumes the Native D3D12 frame buffer is stored in the ```externalTextureD3D12Resource``` variable and that the ```d3d12Device``` is the ```ID3D12Device``` used to initialize Babylon Native.

```cpp

// Make sure Babylon is done rendering.
babylonDeviceUpdate->Finish();
babylonDevice->FinishRenderingCurrentFrame();

// Get CommandQueue used by Babylon Native.
ID3D12CommandQueue* commandQueue;
UINT size = sizeof(commandQueue);
auto hr = d3d12Device->GetPrivateData(__uuidof(**(&commandQueue)), &size, &commandQueue);
winrt::check_hresult(hr);

// Create a Fence to be able to wait on the CPU.
winrt::com_ptr<ID3D12Fence> fence;
hr = d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
winrt::check_hresult(hr);

auto fenceValue = fence->GetCompletedValue();
auto frameBufferDesc = externalTextureD3D12Resource->GetDesc();

// Create a Command List and an Allocator to submit copy commands to the GPU.
winrt::com_ptr<ID3D12CommandAllocator> allocator;
hr = d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&allocator));
winrt::check_hresult(hr);

winrt::com_ptr<ID3D12GraphicsCommandList> commandList;
hr = d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, allocator.get(), nullptr, IID_PPV_ARGS(&commandList));
winrt::check_hresult(hr);

// Create a CPU visible resource to copy the frame buffer content to.
D3D12_RESOURCE_DESC bufferDesc = {};
bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
bufferDesc.Width = frameBufferDesc.Width * frameBufferDesc.Height * 4; // Assuming 4 bytes per pixel (RGBA8 format)
bufferDesc.Height = 1;
bufferDesc.DepthOrArraySize = 1;
bufferDesc.MipLevels = 1;
bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
bufferDesc.SampleDesc.Count = 1;
bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

D3D12_HEAP_PROPERTIES readbackHeapProps = {};
readbackHeapProps.Type = D3D12_HEAP_TYPE_READBACK;

winrt::com_ptr<ID3D12Resource> readbackBuffer;
hr = d3d12Device->CreateCommittedResource(
	&readbackHeapProps,
	D3D12_HEAP_FLAG_NONE,
	&bufferDesc,
	D3D12_RESOURCE_STATE_COPY_DEST,
	nullptr,
	IID_PPV_ARGS(&readbackBuffer)
);

winrt::check_hresult(hr);

// Transition render target to copy source state
D3D12_RESOURCE_BARRIER barrier = {};
barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
barrier.Transition.pResource = g_frameBuffer->GetD3DResource();
barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
commandList->ResourceBarrier(1, &barrier);

// Copy the render target texture to the readback buffer
D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
srcLocation.pResource = g_frameBuffer->GetD3DResource();
srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
srcLocation.SubresourceIndex = 0;

D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
dstLocation.pResource = readbackBuffer.get();
dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
d3d12Device->GetCopyableFootprints(&frameBufferDesc, 0, 1, 0, &dstLocation.PlacedFootprint, nullptr, nullptr, nullptr);

commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

// Transition render target back to render target state
barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
commandList->ResourceBarrier(1, &barrier);
commandList->Close();

// Execute the command list and wait for the GPU to finish
commandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)commandList.put_void());
HANDLE fenceEvent = CreateEventExA(NULL, NULL, 0, EVENT_ALL_ACCESS);
hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
winrt::check_hresult(hr);

hr = commandQueue->Signal(fence.get(), fenceValue++);
winrt::check_hresult(hr);

if (fenceEvent)
{
	WaitForSingleObject(fenceEvent, INFINITE);
}

// Map the readback buffer to CPU readble memory.
void* YOUR_DATA;
D3D12_RANGE readRange = { 0, frameBufferDesc.Width * frameBufferDesc.Height * 4 }; // Assuming 4 bytes per pixel (RGBA8 format)
hr = readbackBuffer->Map(0, &readRange, &YOUR_DATA);
winrt::check_hresult(hr);

// Unmap the readback buffer
readbackBuffer->Unmap(0, nullptr);

// If the application needs to render more frames, release the JS Thread to run the next frame.
babylonDevice->StartRenderingCurrentFrame();
babylonDeviceUpdate->Start();

```
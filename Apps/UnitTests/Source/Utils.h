#pragma once

#include <Babylon/Graphics/Device.h>

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize = 1);
void DestroyTestTexture(Babylon::Graphics::TextureT texture);

// Returns a graphics device suitable for use as Babylon::Graphics::Configuration::Device.
//
// The returned handle is owned by the caller and must be released with DestroyGraphicsDeviceForTest.
//
// Per-platform behaviour:
//   D3D11   -> a fresh ID3D11Device created via D3D11CreateDevice(WARP). Each call returns a distinct handle.
//   D3D12   -> a fresh ID3D12Device created via D3D12CreateDevice on the WARP DXGI adapter. Each call returns a distinct handle.
//   Metal   -> the system-default MTL::Device (MTLCreateSystemDefaultDevice). Metal devices are singleton-per-GPU,
//              so on single-GPU machines successive calls return the same handle. There is no Metal API that
//              creates a second device for the same GPU.
//   OpenGL  -> a GLXContext created via glXCreateNewContext on the test app's X Display. Each call returns a
//              distinct handle. Returns nullptr if the test environment cannot construct a GLX context.
Babylon::Graphics::DeviceT CreateGraphicsDeviceForTest();

void DestroyGraphicsDeviceForTest(Babylon::Graphics::DeviceT device);

#ifdef GRAPHICS_BACK_BUFFER_SUPPORT
// Creates a back-buffer color render target on the given D3D11 device.
//
// The returned RTV holds the only reference to the underlying texture, so releasing it with
// DestroyBackBufferForTest also frees the texture.
Babylon::Graphics::BackBufferColorT CreateBackBufferForTest(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height);

void DestroyBackBufferForTest(Babylon::Graphics::BackBufferColorT backBuffer);
#endif

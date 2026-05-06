#pragma once

#include <Babylon/Graphics/Device.h>

Babylon::Graphics::TextureT CreateTestTexture(Babylon::Graphics::DeviceT device, uint32_t width, uint32_t height, uint32_t arraySize = 1);
void DestroyTestTexture(Babylon::Graphics::TextureT texture);

// Returns a graphics device suitable for use as Babylon::Graphics::Configuration::Device.
//
// The returned handle is owned by the caller and must be released with DestroyGraphicsDeviceForTest.
//
// Defined only on D3D11 and D3D12 -- the Tests.Device.cpp test that consumes these helpers is
// gated to those backends. On D3D11 returns a fresh ID3D11Device created via
// D3D11CreateDevice(WARP); on D3D12 returns a fresh ID3D12Device created via D3D12CreateDevice on
// the WARP DXGI adapter. Each call returns a distinct handle.
Babylon::Graphics::DeviceT CreateGraphicsDeviceForTest();

void DestroyGraphicsDeviceForTest(Babylon::Graphics::DeviceT device);

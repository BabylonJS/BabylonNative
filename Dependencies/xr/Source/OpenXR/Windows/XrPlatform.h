#pragma once

#include <d3d11.h>

#ifndef XR_USE_GRAPHICS_API_D3D11
#define XR_USE_GRAPHICS_API_D3D11
#endif

#ifndef XR_USE_PLATFORM_WIN32
#define XR_USE_PLATFORM_WIN32
#endif

#include <XrPlatformCommon.h>
#include <XR.h>
#include <array>

namespace xr
{
    inline auto CreateGraphicsBinding(const ExtensionDispatchTable& extensions, XrInstance instance, XrSystemId systemId, void* graphicsDevice)
    {
        // Create the D3D11 device for the adapter associated with the system.
        XrGraphicsRequirementsD3D11KHR graphicsRequirements{ XR_TYPE_GRAPHICS_REQUIREMENTS_D3D11_KHR };
        XrCheck(extensions.xrGetD3D11GraphicsRequirementsKHR(instance, systemId, &graphicsRequirements));

        XrGraphicsBindingD3D11KHR graphicsBinding{ XR_TYPE_GRAPHICS_BINDING_D3D11_KHR };
        graphicsBinding.device = reinterpret_cast<ID3D11Device*>(graphicsDevice);
        return graphicsBinding;
    }

    using SwapchainFormat = DXGI_FORMAT;
    using SwapchainImage = XrSwapchainImageD3D11KHR;

    constexpr std::array<SwapchainFormat, 2> SUPPORTED_COLOR_FORMATS
    {
        DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
    };

    constexpr std::array<SwapchainFormat, 2> SUPPORTED_DEPTH_FORMATS
    {
        DXGI_FORMAT_D16_UNORM,
        DXGI_FORMAT_D24_UNORM_S8_UINT
    };

    inline xr::TextureFormat SwapchainFormatToTextureFormat(SwapchainFormat format)
    {
        switch (format)
        {
        case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
            return xr::TextureFormat::BGRA8_SRGB;
        case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
            return xr::TextureFormat::RGBA8_SRGB;
        case DXGI_FORMAT_D24_UNORM_S8_UINT:
            return xr::TextureFormat::D24S8;
        case DXGI_FORMAT_D16_UNORM:
            return xr::TextureFormat::D16;
        default:
            throw std::runtime_error{ "Unsupported texture format" };
        }
    }

    static constexpr XrPosef IDENTITY_TRANSFORM{ XrQuaternionf{ 0.f, 0.f, 0.f, 1.f }, XrVector3f{ 0.f, 0.f, 0.f } };

    constexpr auto SWAPCHAIN_IMAGE_TYPE_ENUM{ XR_TYPE_SWAPCHAIN_IMAGE_D3D11_KHR };

    constexpr std::array<const char*, 1> REQUIRED_EXTENSIONS
    {
        XR_KHR_D3D11_ENABLE_EXTENSION_NAME
    };
}
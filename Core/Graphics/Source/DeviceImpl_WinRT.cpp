#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"
#include <Windows.UI.Core.h>
#include <Windows.UI.Xaml.Controls.h>
#include <Microsoft.UI.Xaml.Controls.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Babylon::Graphics
{
    const bool DeviceImpl::s_bgfxFlipAfterRender = false;

    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        // Assume window is a xaml swap chain panel if not a core window.
        if (!window.try_as<ABI::Windows::UI::Core::ICoreWindow>())
        {
            // Set ndt greater than 1 for xaml swap chain panels.
            // See https://github.com/bkaradzic/bgfx/blob/23edb9c4d90744bf90a89ff9e7308b8ff6517fee/src/dxgi.cpp#L531-L552
            pd.ndt = reinterpret_cast<void*>(2);
        }

        pd.nwh = winrt::get_abi(window);
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT window)
    {
        if (auto uiElement = window.try_as<ABI::Microsoft::UI::Xaml::IUIElement>())
        {
            // Use the ABI layer manually generated from the Windows App SDK winmd files to avoid C++/WinRT versioning issues.
            DOUBLE rasterizationScale{1};
            winrt::check_hresult(uiElement->get_RasterizationScale(&rasterizationScale));
            return static_cast<float>(rasterizationScale);
        }
        else
        {
            return static_cast<float>(winrt::Windows::Graphics::Display::DisplayInformation::GetForCurrentView().RawPixelsPerViewPixel());
        }
    }
}
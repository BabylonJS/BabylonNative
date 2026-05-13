#include "RuntimeImpl.h"

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace Babylon::Integrations
{
    ViewImpl::QuerySizeResult ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        // WindowT here is `winrt::Windows::Foundation::IInspectable`
        // wrapping one of: ICoreWindow, ISwapChainPanel,
        // Microsoft::UI::Xaml::Controls::ISwapChainPanel.
        //
        // Both branches return logical (DIP) units; `View::Attach`
        // converts to physical via the centralized
        // `Babylon::Graphics::GetDevicePixelRatio(window)` (which on
        // IUIElement-capable WinRT windows is `RasterizationScale`).
        // This keeps a single DPR source of truth across the
        // Integrations layer.
        if (auto coreWindow = window.try_as<winrt::Windows::UI::Core::ICoreWindow>())
        {
            const auto bounds = coreWindow.Bounds();
            return {static_cast<uint32_t>(bounds.Width),
                    static_cast<uint32_t>(bounds.Height),
                    CoordinateUnits::Logical};
        }
        if (auto panel = window.try_as<winrt::Windows::UI::Xaml::Controls::ISwapChainPanel>())
        {
            // FrameworkElement.ActualWidth/Height are in DIPs.
            const auto fe = panel.as<winrt::Windows::UI::Xaml::IFrameworkElement>();
            return {static_cast<uint32_t>(fe.ActualWidth()),
                    static_cast<uint32_t>(fe.ActualHeight()),
                    CoordinateUnits::Logical};
        }
        return {0, 0, CoordinateUnits::Logical};
    }
}

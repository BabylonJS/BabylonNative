#include "RuntimeImpl.h"

#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace Babylon::Integrations
{
    std::pair<uint32_t, uint32_t> ViewImpl::QuerySize(Babylon::Graphics::WindowT window)
    {
        // WindowT here is `winrt::Windows::Foundation::IInspectable`
        // wrapping one of: ICoreWindow, ISwapChainPanel,
        // Microsoft::UI::Xaml::Controls::ISwapChainPanel.
        if (auto coreWindow = window.try_as<winrt::Windows::UI::Core::ICoreWindow>())
        {
            const auto bounds = coreWindow.Bounds();
            // CoreWindow.Bounds is in DIPs; multiply by DPI scale to get physical pixels.
            // For now we return DIPs-as-pixels; hosts targeting hi-DPI may
            // adjust by RasterizationScale / DisplayInformation.LogicalDpi.
            return {static_cast<uint32_t>(bounds.Width),
                    static_cast<uint32_t>(bounds.Height)};
        }
        if (auto panel = window.try_as<winrt::Windows::UI::Xaml::Controls::ISwapChainPanel>())
        {
            const auto fe = panel.as<winrt::Windows::UI::Xaml::IFrameworkElement>();
            return {static_cast<uint32_t>(fe.ActualWidth() * panel.CompositionScaleX()),
                    static_cast<uint32_t>(fe.ActualHeight() * panel.CompositionScaleY())};
        }
        return {0, 0};
    }

    std::pair<float, float> ViewImpl::ToLogicalCoords(float x, float y) const
    {
        // UWP `PointerPoint.Position` is in DIPs (logical) — passthrough.
        return {x, y};
    }
}

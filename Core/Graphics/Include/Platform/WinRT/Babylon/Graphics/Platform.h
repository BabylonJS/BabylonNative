#pragma once

#include <stdint.h>
#include <Unknwn.h>
#include <winrt/base.h>

namespace Babylon::Graphics
{
    // Supported types:
    // - Windows::Core::UI::ICoreWindow
    // - Windows::UI::Xaml::Controls::ISwapChainPanel
    // - Microsoft::UI::Xaml::Controls::ISwapChainPanel
    using WindowT = winrt::Windows::Foundation::IInspectable;
}

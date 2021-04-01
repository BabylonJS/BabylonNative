#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <Babylon/Graphics.h>

using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel*;

struct Babylon::GraphicsConfiguration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};

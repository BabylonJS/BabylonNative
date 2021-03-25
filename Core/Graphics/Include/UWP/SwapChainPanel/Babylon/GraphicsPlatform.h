#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

using WindowType = winrt::Windows::UI::Xaml::Controls::ISwapChainPanel*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    size_t width;
    size_t height;
};
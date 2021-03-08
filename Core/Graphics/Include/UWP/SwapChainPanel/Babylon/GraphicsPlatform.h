#pragma once

#include <winrt/Windows.UI.Xaml.Controls.h>

using WindowType = ABI::Windows::UI::Xaml::Controls::ISwapChainPanel*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    void* windowTypePtr;
    size_t width;
    size_t height;
};
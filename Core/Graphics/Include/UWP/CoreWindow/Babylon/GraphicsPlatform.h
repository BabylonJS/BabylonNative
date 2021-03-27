#pragma once

#include <Babylon/Graphics.h>
#include <windows.ui.core.h>
#include <winrt/windows.ui.core.h>
#include <winrt/Windows.ApplicationModel.h>

using WindowType = winrt::Windows::UI::Core::ICoreWindow*;

struct Babylon::Graphics::Configuration
{
    WindowType WindowPtr;
    size_t Width;
    size_t Height;
};

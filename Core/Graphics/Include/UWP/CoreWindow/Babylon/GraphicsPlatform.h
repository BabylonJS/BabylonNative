#pragma once

#include <windows.ui.core.h>
#include <winrt/windows.ui.core.h>
#include <winrt/Windows.ApplicationModel.h>

using WindowType = winrt::Windows::UI::Core::ICoreWindow*;

struct GraphicsConfiguration
{
    WindowType windowPtr;
    size_t width;
    size_t height;
};
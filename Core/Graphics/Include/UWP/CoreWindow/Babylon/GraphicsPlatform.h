#pragma once

#include <windows.ui.core.h>
#include <winrt/windows.ui.core.h>
#include <winrt/Windows.ApplicationModel.h>

namespace Babylon
{
    using WindowType = winrt::Windows::UI::Core::ICoreWindow*;
    using ContextType = ID3D11DeviceContext*;

    struct GraphicsConfiguration
    {
        WindowType WindowPtr;
        size_t Width;
        size_t Height;
    };

    struct ContextConfiguration
    {
        ContextType Context;
        float DefaultPixelRatio{1.f};
    };
}

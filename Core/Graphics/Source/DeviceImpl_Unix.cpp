#include <Babylon/Graphics/Platform.h>
#include <Babylon/Graphics/DeviceQueries.h>
#include "DeviceImpl.h"

#include <X11/Xresource.h>

#include <cmath>
#include <cstdlib>

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = reinterpret_cast<void*>(window);
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::PlatformData& /*pd*/, bgfx::RendererType::Enum& /*renderType*/)
    {
    }

    float GetDevicePixelRatio(WindowT)
    {
        // The device pixel ratio is the desktop environment's scale factor
        // (physical pixels per logical pixel), not the raw monitor pixel
        // density. On X11 the configured scale is published as the "Xft.dpi"
        // X resource (set by GNOME/KDE and tools such as xrandr/xsettingsd),
        // so DPR = Xft.dpi / 96.
        //
        // We deliberately avoid deriving the ratio from DisplayWidthMM/EDID:
        // physical-size reporting is unreliable, is absent under Xvfb, and
        // pixel density is not the same thing as the user's chosen scale.
        //
        // Falls back to GDK_SCALE, then to 1.0 when nothing is configured
        // (e.g. Xvfb, plain X sessions). The result is rounded to the nearest
        // quarter so a value such as 96.02 dpi resolves to an exact 1.0 rather
        // than 1.0002 (which would otherwise shrink a 600 px surface to 599 px).
        //
        // TODO: We should persist a Display object instead of opening a new display.
        // See https://github.com/BabylonJS/BabylonNative/issues/625

        const auto roundToQuarter = [](float value) {
            return std::round(value * 4.0f) / 4.0f;
        };

        if (Display* display = XOpenDisplay(nullptr))
        {
            float dpr = 0.0f;

            if (const char* resourceString = XResourceManagerString(display))
            {
                XrmInitialize();
                if (XrmDatabase database = XrmGetStringDatabase(resourceString))
                {
                    char* type = nullptr;
                    XrmValue value{};
                    if (XrmGetResource(database, "Xft.dpi", "Xft.Dpi", &type, &value) && value.addr != nullptr)
                    {
                        const float dpi = std::strtof(value.addr, nullptr);
                        if (dpi > 0.0f)
                        {
                            dpr = dpi / 96.0f;
                        }
                    }
                    XrmDestroyDatabase(database);
                }
            }

            XCloseDisplay(display);

            if (dpr > 0.0f)
            {
                return roundToQuarter(dpr);
            }
        }

        if (const char* gdkScale = std::getenv("GDK_SCALE"))
        {
            const float scale = std::strtof(gdkScale, nullptr);
            if (scale > 0.0f)
            {
                return roundToQuarter(scale);
            }
        }

        return 1.0f;
    }
}

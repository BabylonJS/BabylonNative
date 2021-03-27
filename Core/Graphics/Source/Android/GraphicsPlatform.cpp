#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

#include <AndroidExtensions/Globals.h>
#include <AndroidExtensions/JavaWrappers.h>

namespace Babylon
{
    void GraphicsImpl::ConfigureBgfxPlatformData(const Graphics::Configuration& config, bgfx::PlatformData& pd)
    {
        pd.ndt = nullptr;
        pd.nwh = config.WindowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    float GraphicsImpl::UpdateDevicePixelRatio()
    {
        // In Android, the baseline DPI is 160dpi.
        // See https://developer.android.com/training/multiscreen/screendensities#dips-pels
        auto dpi = android::global::GetAppContext().getResources().getConfiguration().getDensityDpi();
        std::scoped_lock lock{m_state.Mutex};
        m_state.Resolution.DevicePixelRatio = (float)dpi/160.0f;
        return m_state.Resolution.DevicePixelRatio;
    }
}

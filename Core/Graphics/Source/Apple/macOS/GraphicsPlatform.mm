#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

#include <AppKit/Appkit.h>

namespace Babylon
{

    void Graphics::Impl::ConfigureBgfxPlatformData(GraphicsConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.ndt = nullptr;
        pd.nwh = config.windowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        std::scoped_lock lock{m_state.Mutex};
        MTKView* view = (MTKView*)GetNativeWindow();
        m_state.Resolution.DevicePixelRatio = view.window.screen.backingScaleFactor;
        return m_state.Resolution.DevicePixelRatio;
    }
}

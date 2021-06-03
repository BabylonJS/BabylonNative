#include <Babylon/GraphicsPlatform.h>
#include <GraphicsImpl.h>

#include <AppKit/Appkit.h>
#include <MetalKit/MetalKit.h>

namespace Babylon
{

    void GraphicsImpl::ConfigureBgfxPlatformData(const WindowConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.ndt = nullptr;
        pd.nwh = config.WindowPtr;
        pd.context = nullptr;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    void GraphicsImpl::ConfigureBgfxPlatformData(const ContextConfiguration& config, bgfx::PlatformData& pd)
    {
        pd.ndt = nullptr;
        pd.nwh = nullptr;
        pd.context = config.Context;
        pd.backBuffer = nullptr;
        pd.backBufferDS = nullptr;
    }

    float GraphicsImpl::UpdateDevicePixelRatio()
    {
        std::scoped_lock lock{m_state.Mutex};
        MTKView* view = GetNativeWindow<WindowType>();
        m_state.Resolution.DevicePixelRatio = view.window.screen.backingScaleFactor;
        return m_state.Resolution.DevicePixelRatio;
    }
}

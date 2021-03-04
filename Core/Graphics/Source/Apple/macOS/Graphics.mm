#include <Babylon/Graphics.h>
#include "../GraphicsImpl.h"

#include <MetalKit/MetalKit.h>
#include <AppKit/Appkit.h>

namespace Babylon
{
    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        std::scoped_lock lock{m_state.Mutex};
        MTKView* view = (MTKView*)GetNativeWindow();
        m_state.Resolution.DevicePixelRatio = view.window.screen.backingScaleFactor;
        return m_state.Resolution.DevicePixelRatio;
    }
}

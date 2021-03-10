#include <Babylon/Graphics.h>
#include <Babylon/GraphicsPlatform.h>
#include "../GraphicsImpl.h"

#include <UIKit/UIkit.h>

namespace Babylon
{
    float Graphics::Impl::UpdateDevicePixelRatio()
    {
        std::scoped_lock lock{m_state.Mutex};
        MTKView* view = (MTKView*)GetNativeWindow();
        m_state.Resolution.DevicePixelRatio = view.contentScaleFactor;
        return m_state.Resolution.DevicePixelRatio;
    }
}

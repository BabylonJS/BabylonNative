#include "DeviceImpl.h"
#include <Babylon/Graphics/Platform.h>

namespace Babylon::Graphics {
const bool DeviceImpl::s_bgfxFlipAfterRender = false;

void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData &pd,
                                           WindowT window) {
  pd.nwh = reinterpret_cast<void *>(window);
}

float DeviceImpl::GetDevicePixelRatio(WindowT) { return 1; }
} // namespace Babylon::Graphics

#include <cmath>
#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>

namespace Babylon::Graphics
{
    void DeviceImpl::ConfigureBgfxPlatformData(bgfx::PlatformData& pd, WindowT window)
    {
        pd.nwh = window;
    }

    void DeviceImpl::ConfigureBgfxRenderType(bgfx::PlatformData& /*pd*/, bgfx::RendererType::Enum& /*renderType*/)
    {
    }

    float DeviceImpl::GetDevicePixelRatio(WindowT window)
    {
        // contentsScale can return 0 if it hasn't been set yet.
        // Fallback to the scale from the main screen.
        float scale = static_cast<float>(((CAMetalLayer*)window).contentsScale);
        if (std::isinf(scale) || scale <= 0)
        {
            // UIScreen.mainScreen is deprecated in iOS 26.
            // Prefer getting the scale from the active window scene's trait collection.
            if (@available(iOS 17.0, *))
            {
                for (UIScene* scene in UIApplication.sharedApplication.connectedScenes)
                {
                    if ([scene isKindOfClass:[UIWindowScene class]])
                    {
                        scale = static_cast<float>(((UIWindowScene*)scene).traitCollection.displayScale);
                        if (scale > 0) break;
                    }
                }
            }

            // Fallback for older iOS versions or if no active scene was found.
            if (std::isinf(scale) || scale <= 0)
            {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
                scale = UIScreen.mainScreen.scale;
#pragma clang diagnostic pop
            }
        }
        return scale;
    }
}

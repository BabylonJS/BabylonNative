#include <cmath>
#include <Babylon/Graphics/Platform.h>
#include "DeviceImpl.h"

#import <QuartzCore/QuartzCore.h>
#import <UIKit/UIKit.h>


namespace
{
    bool IsValidScale(float scale)
    {
        return !std::isinf(scale) && scale > 0;
    }
}

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
        float scale = static_cast<float>(((CAMetalLayer*)window).contentsScale);
        if (IsValidScale(scale))
        {
            return scale;
        }

        // Prefer getting the scale from the active window scene's trait collection.
        if (@available(iOS 17.0, *))
        {
            for (UIScene* scene in UIApplication.sharedApplication.connectedScenes)
            {
                if ([scene isKindOfClass:[UIWindowScene class]])
                {
                    scale = static_cast<float>(((UIWindowScene*)scene).traitCollection.displayScale);
                    if (IsValidScale(scale))
                    {
                        return scale;
                    }
                }
            }
        }

        // Fallback for older iOS versions or if no active scene was found.
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        return UIScreen.mainScreen.scale;
#pragma clang diagnostic pop
    }
}

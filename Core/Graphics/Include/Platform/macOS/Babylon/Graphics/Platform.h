#pragma once

#import <MetalKit/MTKView.h>

namespace Babylon::Graphics
{
    using WindowType = MTKView*;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
        // MSAA sample values can be 2, 4, 8 or 16. Any other value will disable MSAA
        unsigned char MSAASamples{};
    };
}

#pragma once

#import <MetalKit/MTKView.h>

namespace Babylon
{
    using WindowType = MTKView*;

    struct WindowConfiguration
    {
        WindowType Window{};
        size_t Width{};
        size_t Height{};
    };
}

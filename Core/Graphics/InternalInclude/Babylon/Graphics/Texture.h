#pragma once

#include <bgfx/bgfx.h>

namespace Babylon::Graphics
{
    struct TextureData final
    {
        ~TextureData();

        void Dispose();

        bgfx::TextureHandle Handle{bgfx::kInvalidHandle};
        bool OwnsHandle{true};
        uint32_t Width{0};
        uint32_t Height{0};
        uint32_t Flags{0};

        // CreationFlags contains flags used at texture creation
        // regarding BLIT support and READBACK
        uint64_t CreationFlags{0};
    };
}

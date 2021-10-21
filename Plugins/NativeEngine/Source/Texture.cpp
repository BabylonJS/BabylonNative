#include "Texture.h"

namespace Babylon
{
    TextureData::~TextureData()
    {
        Dispose();
    }

    void TextureData::Dispose()
    {
        if (OwnsHandle && bgfx::isValid(Handle))
        {
            bgfx::destroy(Handle);
            Handle = BGFX_INVALID_HANDLE;
            OwnsHandle = false;
        }
    }
}

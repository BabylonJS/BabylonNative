#include "Texture.h"

namespace Babylon
{
    TextureData::~TextureData()
    {
        if (OwnsHandle && bgfx::isValid(Handle))
        {
            bgfx::destroy(Handle);
            OwnsHandle = false;
        }
    }
}

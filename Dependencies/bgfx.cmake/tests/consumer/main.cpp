#include <bgfx/bgfx.h>

int main()
{
    return bgfx::getSupportedRenderers(0, nullptr) == 0;
}

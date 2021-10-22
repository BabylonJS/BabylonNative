#include "../Shared/NativeInput.h"

namespace Babylon::Plugins
{
    bool NativeInput::Impl::HasMouse()
    {
        // Assume a mouse is present (or find a way to check this programmatically).
        return true;
    }
}
#include "../Shared/NativeInput.h"

namespace Babylon::Plugins
{
    bool NativeInput::Impl::HasMouse()
    {
        // TODO: Currently assuming a mouse is present.  Need to evaluate possibly detecting this programmatically.
        return true;
    }
}

#include "StandardStreamLoggerPlatform.h"

// Plain Unix already exposes stdout/stderr to the process environment (terminals,
// journald, etc.), so there is nothing to redirect. Start/Stop are successful
// no-ops; the shared API still flips IsStarted() for a uniform host contract.

namespace Babylon::StandardStreamLogger::Platform
{
    bool Start()
    {
        return true;
    }

    bool Stop()
    {
        return true;
    }
}

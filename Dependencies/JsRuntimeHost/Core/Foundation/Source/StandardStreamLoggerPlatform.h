#pragma once

// Internal platform hooks for StandardStreamLogger.
// Each JSRUNTIMEHOST_PLATFORM TU implements these; the shared API TU owns the
// process-wide mutex / started flag and is the only public entry point.

namespace Babylon::StandardStreamLogger::Platform
{
    // Install stdout/stderr redirection and drain threads.
    // Not synchronized — the shared API holds the process-wide mutex.
    bool Start();

    // Flush, restore original streams, and join (or timeout-detach) drains.
    // Not synchronized — the shared API holds the process-wide mutex.
    bool Stop();
}

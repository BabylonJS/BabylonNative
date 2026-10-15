#include "StandardStreamLogger.h"
#include "StandardStreamLoggerPlatform.h"

#include <cstdlib>
#include <mutex>

namespace
{
    std::mutex g_mutex{};
    bool g_started{};
    bool g_exitHandlerRegistered{};
}

namespace Babylon::StandardStreamLogger
{
    bool Start()
    {
        std::lock_guard<std::mutex> lock{g_mutex};
        if (g_started)
        {
            return true;
        }

        if (!Platform::Start())
        {
            return false;
        }

        if (!g_exitHandlerRegistered)
        {
            if (std::atexit([] {
                    (void)Babylon::StandardStreamLogger::Stop();
                }) != 0)
            {
                (void)Platform::Stop();
                return false;
            }
            g_exitHandlerRegistered = true;
        }

        g_started = true;
        return true;
    }

    bool Stop()
    {
        std::lock_guard<std::mutex> lock{g_mutex};
        if (!g_started)
        {
            return true;
        }

        const bool stopped = Platform::Stop();
        g_started = false;
        return stopped;
    }

    bool IsStarted()
    {
        std::lock_guard<std::mutex> lock{g_mutex};
        return g_started;
    }
}
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <functional>

namespace
{
    bool g_debugTraceEnabled{};
    std::function<void(const char* output)> g_traceOutputFunction;
    void TraceArgs(const char* format, va_list args)
    {
        char temp[8192];
        auto len = vsnprintf(temp, sizeof(temp), format, args);
        if (len >= 0 && len < sizeof(temp))
        {
            temp[len] = '\0';
            g_traceOutputFunction(temp);
        }
        else
        {
            g_traceOutputFunction("Error while printing trace string.");
        }
    }
}

namespace Babylon
{
    namespace DebugTrace
    {
        void EnableDebugTrace(bool enabled)
        {
            g_debugTraceEnabled = enabled;
        }

        void Trace(const char* format, ...)
        {
            if (!(g_debugTraceEnabled && g_traceOutputFunction))
            {
                return;
            }
            va_list args;
            va_start(args, format);
            TraceArgs(format, args);
            va_end(args);
        }

        void SetTraceOutput(std::function<void(const char* trace)> traceOutputFunction)
        {
            g_traceOutputFunction = std::move(traceOutputFunction);
        }
    }
}
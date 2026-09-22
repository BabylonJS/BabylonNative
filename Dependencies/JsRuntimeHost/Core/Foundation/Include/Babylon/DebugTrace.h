#pragma once
#include <functional>

#ifdef BABYLON_DEBUG_TRACE
#define _STRINGIFY(x) #x
#define _TOSTRING(x) _STRINGIFY(x)
#define _DEBUG_TRACE(format, ...) Babylon::DebugTrace::Trace(__FILE__ "(" _TOSTRING(__LINE__) "): " format "\n", ##__VA_ARGS__); 

#define DEBUG_TRACE _DEBUG_TRACE
#else // No debugging trace enabled
#define DEBUG_TRACE(...)
#endif // BABYLON_DEBUG_TRACE

namespace Babylon
{
    namespace DebugTrace
    {
        // off by default.
        void EnableDebugTrace(bool enabled);
        // equivalent of a printf that will use TraceOutput function to log. Need Debug Trace to be enabled.
        void Trace(const char* format, ...);
        // set the function used to output.
        void SetTraceOutput(std::function<void(const char* trace)> traceOutputFunction);
    }
}
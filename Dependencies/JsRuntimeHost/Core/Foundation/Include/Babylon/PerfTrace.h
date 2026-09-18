#pragma once
#include <memory>
#include <napi/env.h>


namespace Babylon
{
    namespace PerfTrace
    {
        enum class Level
        {
            // Tracing is a no-op.
            None,
            // Traces time intervals with platform specific profiling APIs.
            Mark,
            // Traces time intervals and also performas platform specific logging.
            Log,
        };

        // Controls a perf trace interval. When the handle is destructed, the perf interval ends.
        class Handle
        {
        public:
            ~Handle();
            Handle(const Handle&) = delete;
            Handle& operator=(const Handle&) = delete;
            Handle(Handle&&) noexcept;
            Handle& operator=(Handle&&) noexcept;

            // Transfers ownership of the handle to a napi value. After this call, destructing the passed in handle has no effect.
            static Napi::Value ToNapi(Napi::Env env, Handle handle);

            // Transfers ownership of the handle from a napi value. After this call, destructing the returned handle will end the perf trace interval.
            static Handle FromNapi(Napi::Value napiValue);

        private:
            friend Handle Trace(const char* name);
            Handle(const char* name);
            class Impl;
            std::unique_ptr<Impl> m_impl;
        };

        // Sets the trace level. It is None by default.
        void SetLevel(Level level);

        // Starts a perf trace interval. Destructing the returned handle ends the perf trace interval.
        Handle Trace(const char* name);
    }
}

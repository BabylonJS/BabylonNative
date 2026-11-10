#include "PerfTrace.h"
#include <arcana/tracing/trace_region.h>
#include <cstdint>
#include <unordered_map>

namespace
{
    std::unordered_map<std::uint32_t, Babylon::PerfTrace::Handle> g_traceHandles;
    std::uint32_t g_nextTraceId = 0;
}

namespace Babylon
{
    namespace PerfTrace
    {
        void SetLevel(Level level)
        {
            switch(level)
            {
                case Level::None:
                    arcana::trace_region::disable();
                    break;
                case Level::Mark:
                    arcana::trace_region::enable(arcana::trace_level::mark);
                    break;
                case Level::Log:
                    arcana::trace_region::enable(arcana::trace_level::log);
                    break;
            }
        }

        // Private implementation to hide arcana::trace_region
        class Handle::Impl
        {
        public:
            Impl(const char* name) : m_region(name) {}
        private:
            arcana::trace_region m_region;
        };

        Handle::Handle(const char* name)
            : m_impl(std::make_unique<Impl>(name))
        {
        }

        Handle::~Handle() = default;
        Handle::Handle(Handle&&) noexcept = default;
        Handle& Handle::operator=(Handle&&) noexcept = default;

        Handle Trace(const char* name)
        {
            return Handle(name);
        }

        Napi::Value Handle::ToNapi(Napi::Env env, Handle traceHandle)
        {
            g_nextTraceId++;
            g_traceHandles.emplace(g_nextTraceId, std::move(traceHandle));
            return Napi::Value::From(env, g_nextTraceId);
        }

        Handle Handle::FromNapi(Napi::Value napiValue)
        {
            const std::uint32_t traceId = napiValue.As<Napi::Number>().Uint32Value();
            auto it = g_traceHandles.find(traceId);
            if (it == g_traceHandles.end())
            {
                throw std::runtime_error("Invalid TraceHandle ID");
            }

            Handle traceHandle = std::move(it->second);
            g_traceHandles.erase(it);
            return traceHandle;
        }
    }
}

#pragma once

#include <stddef.h>

#include "V8Inc.h"

namespace Babylon
{
    class AgentImpl;

    class V8InspectorAgent
    {
    public:
        explicit V8InspectorAgent(
            v8::Platform& platform,
            v8::Isolate* isolate,
            v8::Local<v8::Context> context,
            const char* context_name);

        ~V8InspectorAgent();

        void WaitForDebugger();

        void Start(const unsigned short port, const std::string& appName);

        void Stop();

        bool IsStarted();

        bool IsConnected();

    private:
        std::unique_ptr<AgentImpl> impl;
    };
}

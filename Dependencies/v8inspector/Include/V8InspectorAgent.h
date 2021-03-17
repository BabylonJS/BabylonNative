  
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See NOTICE.md for Node.js' project license details
#pragma once

#include <stddef.h>

#include "V8Inc.h"

namespace Babylon 
{
    class AgentImpl;

    class V8InspectorAgent {
    public:
        // TODO :: safe access to platform
        // Note :: Currently we do support one platform/isolate/context per agent..
        // This is enough for our scenarios.
        explicit V8InspectorAgent(
            v8::Platform& platform,
            v8::Isolate* isolate,
            v8::Local<v8::Context> context,
            const char* context_name,
            unsigned short port);
        ~V8InspectorAgent();

        void waitForDebugger();

        void start();
        void stop();

        bool IsStarted();
        bool IsConnected();
        void WaitForDisconnect();
        void FatalException(
            v8::Local<v8::Value> error,
            v8::Local<v8::Message> message);

    private:
        std::shared_ptr<AgentImpl> impl;
    };
}
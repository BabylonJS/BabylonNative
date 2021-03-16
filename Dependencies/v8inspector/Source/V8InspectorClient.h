#pragma once

#include <V8Inc.h>

namespace Babylon
{
    class AgentImpl;

    class V8NodeInspector : public v8_inspector::V8InspectorClient
    {
    public:
        V8NodeInspector(AgentImpl& agent);

        void setupContext(v8::Local<v8::Context> context, const char* context_name /*must be null terminated*/);

        void runMessageLoopOnPause(int /*context_group_id*/) override;

        double currentTimeMS() override;

        void quitMessageLoopOnPause() override;

        void connectFrontend();

        void disconnectFrontend();

        void dispatchMessageFromFrontend(const v8_inspector::StringView& message);

        v8::Local<v8::Context> ensureDefaultContextInGroup(int /*contextGroupId*/) override;

        inline v8_inspector::V8Inspector* inspector() const
        {
            return inspector_.get();
        }

        inline bool isWaitingForResume() const
        {
            return waiting_for_resume_;
        }

        std::unique_ptr<v8_inspector::V8InspectorSession> session_;

    private:
        AgentImpl& agent_;
        v8::Platform* platform_;
        std::atomic<bool> waiting_for_resume_{ false };
        bool running_nested_loop_;
        std::unique_ptr<v8_inspector::V8Inspector> inspector_;
    };
}
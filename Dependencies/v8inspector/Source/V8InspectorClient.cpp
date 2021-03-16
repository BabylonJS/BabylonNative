#include "V8InspectorClient.h"
#include "V8InspectorUtils.h"

#include <chrono>

namespace Babylon
{
    V8NodeInspector::V8NodeInspector(AgentImpl& agent)
        : agent_(agent)
        , waiting_for_resume_(false)
        , running_nested_loop_(false)
        , inspector_(v8_inspector::V8Inspector::create(agent.isolate_, this))
    {
    }

    void V8NodeInspector::setupContext(
        v8::Local<v8::Context> context,
        const char* context_name /*must be null terminated*/)
    {
        std::unique_ptr<v8_inspector::StringBuffer> name_buffer = utils::Utf8ToStringView(context_name);
        v8_inspector::V8ContextInfo info(context, 1, name_buffer->string());

        std::unique_ptr<v8_inspector::StringBuffer> aux_data_buffer = utils::Utf8ToStringView("{\"isDefault\":true}");
        info.auxData = aux_data_buffer->string();

        inspector_->contextCreated(info);
    }

    void V8NodeInspector::runMessageLoopOnPause(int /*context_group_id*/) override
    {
        waiting_for_resume_ = true;
        if (running_nested_loop_)
            return;
        running_nested_loop_ = true;
        while (waiting_for_resume_)
        {
            agent_.WaitForFrontendMessage();
            agent_.DispatchMessages();
        }
        waiting_for_resume_ = false;
        running_nested_loop_ = false;
    }

    double V8NodeInspector::currentTimeMS() override
    {
        auto duration = std::chrono::system_clock::now().time_since_epoch();
        return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
    }

    void V8NodeInspector::quitMessageLoopOnPause() override
    {
        waiting_for_resume_ = false;
    }

    void V8NodeInspector::connectFrontend()
    {
        session_ = inspector_->connect(
            1, new ChannelImpl(agent_), v8_inspector::StringView());
    }

    void V8NodeInspector::disconnectFrontend()
    {
        session_.reset();
    }

    void V8NodeInspector::dispatchMessageFromFrontend(const v8_inspector::StringView& message)
    {
        std::string messagestr = utils::StringViewToUtf8(message);

        if (agent_.waiting_for_frontend_)
            agent_.waiting_for_frontend_ =
            messagestr.find("Runtime.runIfWaitingForDebugger") !=
            std::string::npos;

        session_->dispatchProtocolMessage(message);
    }

    v8::Local<v8::Context> V8NodeInspector::ensureDefaultContextInGroup(int /*contextGroupId*/) override
    {
        return v8::Isolate::GetCurrent()->GetCurrentContext();
    }
}
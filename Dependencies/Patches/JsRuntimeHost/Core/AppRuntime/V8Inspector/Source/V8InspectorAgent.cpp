// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
// This code is based on the old node inspector implementation. See NOTICE.md for Node.js' project license details
#include <V8InspectorAgent.h>
#include "V8InspectorSocketServer.h"
#include "V8InspectorUtils.h"

#include <v8-inspector.h>
#include <v8-platform.h>

#include <string.h>
#include <chrono>
#include <map>
#include <sstream>
#include <thread>
#include <utility>
#include <vector>
#include <condition_variable>
#include <array>
#include <random>
#include <algorithm>

namespace Babylon
{
    const char TAG_CONNECT[] = "#connect";
    const char TAG_DISCONNECT[] = "#disconnect";

    std::string GetProcessTitle()
    {
        return "BabylonNative";
    }

    class V8NodeInspector;

    class AgentImpl
    {
    public:
        explicit AgentImpl(
            v8::Platform& platform,
            v8::Isolate* isolate,
            v8::Local<v8::Context> context,
            const char* context_name);
        ~AgentImpl();

        void Start(const unsigned short port, const std::string& appName);
        void Stop();

        void WaitForDebugger();

        bool IsStarted();
        bool IsConnected();

        void PostIncomingMessage(int session_id, const std::string& message);

    private:
        using MessageQueue =
            std::vector<std::pair<int, std::unique_ptr<v8_inspector::StringBuffer>>>;
        enum class State
        {
            kNew,
            kAccepting,
            kConnected,
            kDone,
            kError
        };

        void DispatchMessages();
        void Write(
            int session_id,
            std::unique_ptr<v8_inspector::StringBuffer> message);
        bool AppendMessage(
            MessageQueue* vector,
            int session_id,
            std::unique_ptr<v8_inspector::StringBuffer> buffer);
        void SwapBehindLock(MessageQueue* vector1, MessageQueue* vector2);
        void WaitForFrontendMessage();
        void NotifyMessageReceived();

        std::mutex incoming_message_cond_m_;
        std::condition_variable incoming_message_cond_;

        std::mutex state_m;

        unsigned short port_;

        bool waiting_for_frontend_ = true;

        std::unique_ptr<V8NodeInspector> inspector_;
        v8::Isolate* isolate_;
        MessageQueue incoming_message_queue_;
        MessageQueue outgoing_message_queue_;
        bool dispatching_messages_;
        int session_id_;
        std::unique_ptr<InspectorSocketServer> server_;

        std::string script_name_;

        v8::Platform& platform_;

        friend class ChannelImpl;
        friend class DispatchOnInspectorBackendTask;
        friend class SetConnectedTask;
        friend class V8NodeInspector;
        friend void InterruptCallback(v8::Isolate*, void* agent);

    public:
    };

    void InterruptCallback(v8::Isolate*, void* agent)
    {
        static_cast<AgentImpl*>(agent)->DispatchMessages();
    }

    class DispatchOnInspectorBackendTask : public v8::Task
    {
    public:
        explicit DispatchOnInspectorBackendTask(AgentImpl& agent)
            : agent_(agent)
        {
        }

    private:
        // ------------------
        // v8::Task overrides
        // ------------------

        void Run() override
        {
            agent_.DispatchMessages();
        }

    private:
        AgentImpl& agent_;
    };

    class ChannelImpl final : public v8_inspector::V8Inspector::Channel
    {
    public:
        explicit ChannelImpl(AgentImpl& agent)
            : agent_(agent)
        {
        }
        virtual ~ChannelImpl() {}

    private:
        // --------------------------------------------
        // v8_inspector::V8Inspector::Channel overrides
        // --------------------------------------------

        void sendResponse(
            int /*callId*/,
            std::unique_ptr<v8_inspector::StringBuffer> message) override
        {
            agent_.Write(agent_.session_id_, std::move(message));
        }

        void sendNotification(
            std::unique_ptr<v8_inspector::StringBuffer> message) override
        {
            agent_.Write(agent_.session_id_, std::move(message));
        }

        void flushProtocolNotifications() override {}

    private:
        AgentImpl& agent_;
    };

    using V8Inspector = v8_inspector::V8Inspector;

    class V8NodeInspector : public v8_inspector::V8InspectorClient
    {
    public:
        V8NodeInspector(AgentImpl& agent)
            : agent_(agent)
            , waiting_for_resume_(false)
            , running_nested_loop_(false)
            , inspector_(V8Inspector::create(agent.isolate_, this))
        {
        }

        void SetupContext(
            v8::Local<v8::Context> context,
            const char* context_name /*must be null terminated*/)
        {
            std::unique_ptr<v8_inspector::StringBuffer> name_buffer = utils::Utf8ToStringView(context_name);
            v8_inspector::V8ContextInfo info(context, 1, name_buffer->string());

            std::unique_ptr<v8_inspector::StringBuffer> aux_data_buffer = utils::Utf8ToStringView("{\"isDefault\":true}");
            info.auxData = aux_data_buffer->string();

            inspector_->contextCreated(info);
        }

        void ConnectFrontend()
        {
            session_ = inspector_->connect(
                1, new ChannelImpl(agent_), v8_inspector::StringView()
// v8-android package and v8 nuget do not share the same V8 version. A change in V8_inspector API forces us to add this
// ifndef check. This will be fixed in a future nuget package update.
#ifndef ANDROID
                , v8_inspector::V8Inspector::kFullyTrusted
#endif
            );
        }

        void DisconnectFrontend()
        {
            session_.reset();
        }

        void DispatchMessageFromFrontend(const v8_inspector::StringView& message)
        {
            std::string messagestr = utils::StringViewToUtf8(message);

            if (agent_.waiting_for_frontend_)
                agent_.waiting_for_frontend_ =
                    messagestr.find("Runtime.runIfWaitingForDebugger") !=
                    std::string::npos;

            session_->dispatchProtocolMessage(message);
        }

        V8Inspector* Inspector()
        {
            return inspector_.get();
        }

        bool IsWaitingForResume()
        {
            return waiting_for_resume_;
        }

        void SchedulePauseOnNextStatement(const v8_inspector::StringView& reason, const v8_inspector::StringView& details)
        {
            session_->schedulePauseOnNextStatement(reason, details);
        }

    private:
        // -----------------------------------------
        // v8_inspector::V8InspectorClient overrides
        // -----------------------------------------

        void runMessageLoopOnPause(int /*context_group_id*/) override
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

        double currentTimeMS() override
        {
            auto duration = std::chrono::system_clock::now().time_since_epoch();
            return static_cast<double>(std::chrono::duration_cast<std::chrono::milliseconds>(duration)
                                           .count());
        }

        void quitMessageLoopOnPause() override
        {
            waiting_for_resume_ = false;
        }

        v8::Local<v8::Context> ensureDefaultContextInGroup(
            int /*contextGroupId*/) override
        {
            return v8::Isolate::GetCurrent()->GetCurrentContext();
        }

    private:
        AgentImpl& agent_;
        std::atomic<bool> waiting_for_resume_{false};
        bool running_nested_loop_;
        std::unique_ptr<V8Inspector> inspector_;
        std::unique_ptr<v8_inspector::V8InspectorSession> session_;
    };

    AgentImpl::AgentImpl(
        v8::Platform& platform,
        v8::Isolate* isolate,
        v8::Local<v8::Context> context,
        const char* context_name)
        : inspector_(nullptr)
        , isolate_(isolate)
        , dispatching_messages_(false)
        , session_id_(0)
        , server_(nullptr)
        , platform_(platform)
    {
        inspector_ = std::make_unique<V8NodeInspector>(*this);
        inspector_->SetupContext(context, context_name);
    }

    AgentImpl::~AgentImpl() {}

    void InspectorConsoleCall(const v8::FunctionCallbackInfo<v8::Value>& info)
    {
        v8::Isolate* isolate = info.GetIsolate();
        v8::Local<v8::Context> context = isolate->GetCurrentContext();

        v8::Local<v8::Array> args = info.Data().As<v8::Array>();
        CHECK_EQ(args->Length(), 3);

        v8::Local<v8::Value> inspector_method =
            args->Get(context, 0).ToLocalChecked();
        CHECK(inspector_method->IsFunction());
        v8::Local<v8::Value> node_method = args->Get(context, 1).ToLocalChecked();
        CHECK(node_method->IsFunction());
        v8::Local<v8::Value> config_value = args->Get(context, 2).ToLocalChecked();
        CHECK(config_value->IsObject());
        v8::Local<v8::Object> config_object = config_value.As<v8::Object>();

        std::vector<v8::Local<v8::Value>> call_args(info.Length());
        for (int i = 0; i < info.Length(); ++i)
        {
            call_args[i] = info[i];
        }

        v8::Local<v8::String> in_call_key = utils::OneByteString(isolate, "in_call");
        bool in_call = config_object->Has(context, in_call_key).FromMaybe(false);
        if (!in_call)
        {
            CHECK(
                config_object->Set(context, in_call_key, v8::True(isolate)).FromJust());
            CHECK(
                !inspector_method.As<v8::Function>()
                     ->Call(context, info.Holder(), static_cast<int>(call_args.size()), call_args.data())
                     .IsEmpty());
        }

        v8::TryCatch try_catch(info.GetIsolate());
        static_cast<void>(node_method.As<v8::Function>()->Call(
            context, info.Holder(), static_cast<int>(call_args.size()), call_args.data()));
        CHECK(config_object->Delete(context, in_call_key).FromJust());
        if (try_catch.HasCaught())
            try_catch.ReThrow();
    }

    void InspectorWrapConsoleCall(const v8::FunctionCallbackInfo<v8::Value>& args)
    {

        v8::Local<v8::Array> array =
            v8::Array::New(v8::Isolate::GetCurrent(), args.Length());
        CHECK(array->Set(v8::Isolate::GetCurrent()->GetCurrentContext(), 0, args[0])
                  .FromJust());
        CHECK(array->Set(v8::Isolate::GetCurrent()->GetCurrentContext(), 1, args[1])
                  .FromJust());
        CHECK(array->Set(v8::Isolate::GetCurrent()->GetCurrentContext(), 2, args[2])
                  .FromJust());
        args.GetReturnValue().Set(v8::Function::New(
            v8::Isolate::GetCurrent()->GetCurrentContext(),
            InspectorConsoleCall,
            array).ToLocalChecked());
    }

    void AgentImpl::Start(const unsigned short port, const std::string& appName)
    {
        port_ = port;
        script_name_ = appName;

        // be sure server_ is not still in use here or its allocation will be replace in the thread func
        // this can happen if reusing the same AgentImpl object, stopping and restarting before the InspectorSocketServer is properly pulled down 
        if (server_) {
            throw std::runtime_error("can't start again the server as previous InspectorSocketServer is still active.");
        }

        auto delegate = std::make_unique<InspectorAgentDelegate>(*this, "", script_name_, false);
        server_ = std::make_unique<InspectorSocketServer>(std::move(delegate), port_);
        server_->Start();
    }

    void AgentImpl::WaitForDebugger()
    {
        WaitForFrontendMessage();

        while (waiting_for_frontend_)
            DispatchMessages();

        std::string reasonstr("Break on start");
        v8_inspector::StringView reason(
            reinterpret_cast<const uint8_t*>(reasonstr.c_str()), reasonstr.size()),
            details(
                reinterpret_cast<const uint8_t*>(reasonstr.c_str()),
                reasonstr.size());
        inspector_->SchedulePauseOnNextStatement(reason, details);
    }

    void AgentImpl::Stop()
    {
        if (inspector_)
        {
            inspector_->DisconnectFrontend();
            inspector_.reset();
        }

        if (server_)
        {
            server_->Stop();
        }
    }

    bool AgentImpl::IsConnected()
    {
        return server_ && server_->IsConnected();
    }

    bool AgentImpl::IsStarted()
    {
        return !!server_;
    }

    std::unique_ptr<v8_inspector::StringBuffer> ToProtocolString(
        v8::Local<v8::Value> value)
    {
        if (value.IsEmpty() || value->IsNull() || value->IsUndefined() ||
            !value->IsString())
        {
            return v8_inspector::StringBuffer::create(v8_inspector::StringView());
        }
        v8::Local<v8::String> string_value = v8::Local<v8::String>::Cast(value);
        int len = string_value->Length();
        std::vector<uint16_t> buffer(len, 0);
        string_value->Write(v8::Isolate::GetCurrent(), buffer.data(), 0, len);
        return v8_inspector::StringBuffer::create(
            v8_inspector::StringView(buffer.data(), len));
    }

    bool AgentImpl::AppendMessage(
        MessageQueue* queue,
        int session_id,
        std::unique_ptr<v8_inspector::StringBuffer> buffer)
    {
        std::unique_lock<std::mutex> lock(state_m);
        bool trigger_pumping = queue->empty();
        queue->push_back(std::make_pair(session_id, std::move(buffer)));
        return trigger_pumping;
    }

    void AgentImpl::SwapBehindLock(MessageQueue* vector1, MessageQueue* vector2)
    {
        std::unique_lock<std::mutex> lock(state_m);
        vector1->swap(*vector2);
    }

    void AgentImpl::PostIncomingMessage(
        int session_id,
        const std::string& message)
    {
        if (AppendMessage(&incoming_message_queue_, session_id, utils::Utf8ToStringView(message)))
        {
            std::shared_ptr<v8::TaskRunner> foregroundTaskRunner;

#ifdef USE_DEFAULT_PLATFORM
            // Need to get the foreground runner from the isolate data slot
            v8runtime::IsolateData* isolate_data = reinterpret_cast<v8runtime::IsolateData*>(isolate_->GetData(v8runtime::ISOLATE_DATA_SLOT));
            foregroundTaskRunner = isolate_data->foreground_task_runner_;
#else
            foregroundTaskRunner = platform_.GetForegroundTaskRunner(isolate_);
#endif
            foregroundTaskRunner->PostTask(std::make_unique<DispatchOnInspectorBackendTask>(*this));
            isolate_->RequestInterrupt(InterruptCallback, this);
        }
        NotifyMessageReceived();
    }

    void AgentImpl::WaitForFrontendMessage()
    {
        std::unique_lock<std::mutex> lock(incoming_message_cond_m_);
        if (incoming_message_queue_.empty())
            incoming_message_cond_.wait(
                lock, [this] { return !incoming_message_queue_.empty(); });
    }

    void AgentImpl::NotifyMessageReceived()
    {
        incoming_message_cond_.notify_all();
    }

    void AgentImpl::DispatchMessages()
    {
        // This function can be reentered if there was an incoming message while
        // V8 was processing another inspector request (e.g. if the user is
        // evaluating a long-running JS code snippet). This can happen only at
        // specific points (e.g. the lines that call inspector_ methods)
        if (dispatching_messages_)
            return;
        dispatching_messages_ = true;
        MessageQueue tasks;
        do
        {
            tasks.clear();
            SwapBehindLock(&incoming_message_queue_, &tasks);
            for (const MessageQueue::value_type& pair : tasks)
            {
                v8_inspector::StringView message = pair.second->string();
                std::string tag;
                if (message.length() == sizeof(TAG_CONNECT) - 1 ||
                    message.length() == sizeof(TAG_DISCONNECT) - 1)
                {
                    tag = utils::StringViewToUtf8(message);
                }

                if (tag == TAG_CONNECT)
                {
                    session_id_ = pair.first;
                    inspector_->ConnectFrontend();
                }
                else if (tag == TAG_DISCONNECT)
                {
                    inspector_->DisconnectFrontend();
                }
                else if (inspector_)
                {
                    inspector_->DispatchMessageFromFrontend(message);
                }
                else
                {
#ifdef WIN32
                    OutputDebugStringW(L"Warning: V8 inspector message dropped - ");
                    if (message.is8Bit())
                    {
                        OutputDebugStringA(reinterpret_cast<const char*>(message.characters8()));
                    }
                    else
                    {
                        OutputDebugStringW(reinterpret_cast<const wchar_t*>(message.characters16()));
                    }
                    OutputDebugStringW(L"\n");
#endif
                }
            }
        } while (!tasks.empty());
        dispatching_messages_ = false;
    }

    void AgentImpl::Write(
        int session_id,
        std::unique_ptr<v8_inspector::StringBuffer> inspector_message)
    {
        AppendMessage(
            &outgoing_message_queue_, session_id, std::move(inspector_message));

        MessageQueue outgoing_messages;
        SwapBehindLock(&outgoing_message_queue_, &outgoing_messages);
        for (const MessageQueue::value_type& outgoing : outgoing_messages)
        {
            v8_inspector::StringView view = outgoing.second->string();
            assert(server_);
            if (server_)
            {
                if (view.length() == 0)
                {
                    server_->Stop();
                }
                else
                {
                    server_->Send(
                        outgoing.first, utils::StringViewToUtf8(outgoing.second->string()));
                }
            }
        }
    }

    // Exported class Agent
    V8InspectorAgent::V8InspectorAgent(
        v8::Platform& platform,
        v8::Isolate* isolate,
        v8::Local<v8::Context> context,
        const char* context_name)
        : impl(std::make_unique<AgentImpl>(platform, isolate, context, context_name))
    {
    }

    V8InspectorAgent::~V8InspectorAgent()
    {
    }

    void V8InspectorAgent::WaitForDebugger()
    {
        impl->WaitForDebugger();
    }

    void V8InspectorAgent::Stop()
    {
        impl->Stop();
    }

    void V8InspectorAgent::Start(const unsigned short port, const std::string& appName)
    {
        impl->Start(port, appName);
    }

    bool V8InspectorAgent::IsStarted()
    {
        return impl->IsStarted();
    }

    bool V8InspectorAgent::IsConnected()
    {
        return impl->IsConnected();
    }

    InspectorAgentDelegate::InspectorAgentDelegate(
        AgentImpl& agent,
        const std::string& script_path,
        const std::string& script_name,
        bool wait)
        : agent_(agent)
        , connected_(false)
        , script_name_(script_name)
        , script_path_(script_path)
        , target_id_(utils::GenerateUniqueID())
        , waiting_(wait)
    {
    }

    void InspectorAgentDelegate::StartSession(
        int session_id,
        const std::string& /*target_id*/)
    {
        connected_ = true;
        agent_.PostIncomingMessage(session_id, TAG_CONNECT);
    }

    void InspectorAgentDelegate::MessageReceived(
        int session_id,
        const std::string& message)
    {
        // TODO(pfeldman): Instead of blocking execution while debugger
        // engages, node should wait for the run callback from the remote client
        // and initiate its startup. This is a change to node.cc that should be
        // upstreamed separately.
        if (waiting_)
        {
            if (message.find("\"Runtime.runIfWaitingForDebugger\"") !=
                std::string::npos)
            {
                waiting_ = false;
            }
        }
        agent_.PostIncomingMessage(session_id, message);
    }

    void InspectorAgentDelegate::EndSession(int session_id)
    {
        connected_ = false;
        agent_.PostIncomingMessage(session_id, TAG_DISCONNECT);
    }

    std::vector<std::string> InspectorAgentDelegate::GetTargetIds()
    {
        return {target_id_};
    }

    std::string InspectorAgentDelegate::GetTargetTitle(const std::string& /*id*/)
    {
        return script_name_.empty() ? GetProcessTitle() : script_name_;
    }

    std::string InspectorAgentDelegate::GetTargetUrl(const std::string& /*id*/)
    {
        return "file://" + script_path_;
    }

} // namespace inspector
#pragma once

#include <napi/napi.h>
#include <Babylon/Runtime.h>

namespace babylon
{
    class Console final : public Napi::ObjectWrap<Console>
    {
    public:
        static Napi::ObjectReference Create(Napi::Env env, LogCallback& callback);
        
        explicit Console(const Napi::CallbackInfo& info);

<<<<<<< HEAD
        static void RegisterLogOutput(MessageLogger output);
        static void RegisterWarnOutput(MessageLogger output);
        static void RegisterErrorOutput(MessageLogger output);

        static void Log(const char* message);
        static void Warn(const char* message);
        static void Error(const char* message);

=======
>>>>>>> 69d98f0617a14313c25c2d95a02cd7176f5854c4
    private:
        void Log(const Napi::CallbackInfo& info);
        void Warn(const Napi::CallbackInfo& info);
        void Error(const Napi::CallbackInfo& info);

        void SendToOutputs(const Napi::CallbackInfo&, LogLevel) const;

<<<<<<< HEAD
        void SendToOutputs(const Napi::CallbackInfo& info, std::vector<MessageLogger>& outputs);
        static void SendToOutputs(const char* message, std::vector<MessageLogger>& outputs);
=======
        const LogCallback& m_callback;
>>>>>>> 69d98f0617a14313c25c2d95a02cd7176f5854c4
    };
}

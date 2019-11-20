#pragma once

#include <napi/napi.h>

#include <sstream>

namespace babylon
{
    template<typename LogHandlerT>
    class Console final : public Napi::ObjectWrap<Console<LogHandlerT>>
    {
    public:
        using ParentT = Napi::ObjectWrap<Console<LogHandlerT>>;

        /**
         * Importance level of messages sent via logging callbacks.
         */
        enum class LogLevel
        {
            Log,
            Warn,
            Error,
        };

        static Napi::ObjectReference Create(Napi::Env env, LogHandlerT& handler)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = ParentT::DefineClass(
                env,
                "Console",
                {
                    ParentT::InstanceMethod("log", &Console::Log),
                    ParentT::InstanceMethod("warn", &Console::Warn),
                    ParentT::InstanceMethod("error", &Console::Error),
                });

            return Napi::Persistent(func.New({ Napi::External<LogHandlerT>::New(env, &handler) }));
        }
        
        explicit Console(const Napi::CallbackInfo& info)
            : ParentT{ info }
            , m_handler{ *info[0].As<Napi::External<LogHandlerT>>().Data() }
        {}

    private:
        void Log(const Napi::CallbackInfo& info)
        {
            // TODO: Log output to ETW/telemetry rather than debugger output.
            // TODO: Handle version of this method that takes a format string as the first parameter.
            SendToOutputs(info, LogLevel::Log);
        }

        void Warn(const Napi::CallbackInfo& info)
        {
            SendToOutputs(info, LogLevel::Warn);
        }

        void Error(const Napi::CallbackInfo& info)
        {
            SendToOutputs(info, LogLevel::Error);
        }

        void SendToOutputs(const Napi::CallbackInfo& info, LogLevel logLevel) const
        {
            std::stringstream ss{};
            for (unsigned int index = 0; index < info.Length(); index++)
            {
                if (index > 0)
                {
                    ss << " ";
                }
                ss << info[index].ToString().Utf8Value().c_str();
            }
            ss << std::endl;
            m_handler.Log(ss.str().c_str(), logLevel);
        }

        const LogHandlerT& m_handler;
    };
}

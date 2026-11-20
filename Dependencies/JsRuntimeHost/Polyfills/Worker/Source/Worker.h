#pragma once

#include <Babylon/Polyfills/Worker.h>
#include <napi/napi.h>

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class Worker final : public Napi::ObjectWrap<Worker>
    {
    public:
        static void Initialize(Napi::Env env, Babylon::Polyfills::Worker::Options options);

        explicit Worker(const Napi::CallbackInfo& info);
        ~Worker() override;

    private:
        struct Message
        {
            std::string Json;
            std::vector<std::vector<std::uint8_t>> Buffers;
        };

        struct State;

        Napi::Value PostMessage(const Napi::CallbackInfo& info);
        void Terminate(const Napi::CallbackInfo& info);
        void Stop();

        static Message Serialize(Napi::Env env, const Napi::Value& value, const Napi::Value& transfer);
        static Napi::Value Deserialize(Napi::Env env, const Message& message);
        static void InitializeWorker(const std::shared_ptr<State>& state,
                                     Napi::Env env,
                                     const std::string& url,
                                     bool module);
        static void DispatchMessageToParent(const std::weak_ptr<State>& state, Message message);
        static void DispatchErrorToParent(const std::weak_ptr<State>& state, std::string message);

        std::shared_ptr<State> m_state{};
    };
}

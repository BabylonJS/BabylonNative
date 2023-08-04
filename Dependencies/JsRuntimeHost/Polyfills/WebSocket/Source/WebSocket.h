#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <UrlLib/UrlLib.h>
#include <napi/napi.h>

namespace Babylon::Polyfills::Internal
{
    class WebSocket final : public Napi::ObjectWrap<WebSocket>
    {
    public:
        static void Initialize(Napi::Env env);
        explicit WebSocket(const Napi::CallbackInfo& info);
        virtual ~WebSocket();

    private:
        enum class ReadyState
        {
            Connecting = 0,
            Open = 1,
            Closing = 2,
            Closed = 3
        };

        Napi::Value GetReadyState(const Napi::CallbackInfo& info);
        Napi::Value GetURL(const Napi::CallbackInfo& info);

        void SetOnOpen(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetOnClose(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetOnMessage(const Napi::CallbackInfo& info, const Napi::Value& value);
        void SetOnError(const Napi::CallbackInfo& info, const Napi::Value& value);

        Napi::Value GetOnOpen(const Napi::CallbackInfo& info);
        Napi::Value GetOnClose(const Napi::CallbackInfo& info);
        Napi::Value GetOnMessage(const Napi::CallbackInfo& info);
        Napi::Value GetOnError(const Napi::CallbackInfo& info);

        void OpenCallback();
        void CloseCallback(int code, const std::string& reason);
        void MessageCallback(const std::string& message);
        void ErrorCallback(const std::string& message);

        void Close(const Napi::CallbackInfo& info);
        void Send(const Napi::CallbackInfo& info);

        JsRuntimeScheduler m_runtimeScheduler;

        Napi::FunctionReference m_onopen;
        Napi::FunctionReference m_onclose;
        Napi::FunctionReference m_onmessage;
        Napi::FunctionReference m_onerror;

        UrlLib::WebSocket m_webSocket;
        const std::string m_url{};
        ReadyState m_readyState{ReadyState::Connecting};

        std::shared_ptr<arcana::cancellation_source> m_cancellationSource{};
    };
}

#pragma once

#include <Babylon/JsRuntimeScheduler.h>

#include <napi/napi.h>
#include <UrlLib/UrlLib.h>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class XMLHttpRequest final : public Napi::ObjectWrap<XMLHttpRequest>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit XMLHttpRequest(const Napi::CallbackInfo& info);

    private:
        enum class ReadyState
        {
            Unsent = 0,
            Opened = 1,
            HeadersReceived = 2,
            Loading = 3,
            Done = 4,
        };

        Napi::Value GetReadyState(const Napi::CallbackInfo& info);
        Napi::Value GetResponse(const Napi::CallbackInfo& info);
        Napi::Value GetResponseText(const Napi::CallbackInfo& info);
        Napi::Value GetResponseType(const Napi::CallbackInfo& info);
        void SetResponseType(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetResponseHeader(const Napi::CallbackInfo& info);
        void SetRequestHeader(const Napi::CallbackInfo& info);
        Napi::Value GetAllResponseHeaders(const Napi::CallbackInfo& info);
        Napi::Value GetResponseURL(const Napi::CallbackInfo& info);
        Napi::Value GetStatus(const Napi::CallbackInfo& info);
        Napi::Value GetStatusText(const Napi::CallbackInfo& info);
        Napi::Value GetErrorCode(const Napi::CallbackInfo& info);
        Napi::Value GetErrorDetail(const Napi::CallbackInfo& info);
        Napi::Value GetTimeout(const Napi::CallbackInfo& info);
        void SetTimeout(const Napi::CallbackInfo& info, const Napi::Value& value);
        Napi::Value GetOnHandler(const Napi::CallbackInfo& info);
        void SetOnHandler(const Napi::CallbackInfo& info, const Napi::Value& value);

        void AddEventListener(const Napi::CallbackInfo& info);
        void RemoveEventListener(const Napi::CallbackInfo& info);
        void Abort(const Napi::CallbackInfo& info);
        void Open(const Napi::CallbackInfo& info);
        void Send(const Napi::CallbackInfo& info);

        // Whether a request has been sent and has not settled yet.
        bool IsInFlight() const;

        // Abandons the request of the current open(): what it still delivers is ignored.
        void Supersede();

        // The methods below that raise events take the JavaScript object of this request as
        // `self`: it is the receiver and the target of the events. (ObjectWrap::Value() is not
        // available on every Node-API backend.)

        // Runs when the transfer started by the open() numbered `generation` has settled.
        void OnSettled(const Napi::Object& self, uint32_t generation, bool transportFailed);

        // The "request error steps" of the XMLHttpRequest standard, for a network error ("error"),
        // abort() ("abort") and an expired timeout ("timeout").
        void RequestErrorSteps(const Napi::Object& self, const char* eventType);

        void StartTimeout(const Napi::Object& self);
        void CancelTimeout();

        // Drops the event listeners and handlers once the request numbered `generation` is over.
        void ReleaseHandlers(uint32_t generation);

        void SetReadyState(const Napi::Object& self, ReadyState readyState);
        void RaiseEvent(const Napi::Object& self, const char* eventType, double loaded = 0, double total = 0);

        std::string m_url{};
        UrlLib::UrlRequest m_request{};
        JsRuntimeScheduler m_runtimeScheduler;
        ReadyState m_readyState{ReadyState::Unsent};

        // The send() flag of the XMLHttpRequest standard: set from send() until the request settles.
        bool m_sendFlag{false};
        // m_request has been sent, so the next open() needs a new one.
        bool m_requestSent{false};
        // m_request holds a response. False before the response arrives, after a network error and
        // after abort(); the response members then report their empty values.
        bool m_hasResponse{false};
        // Counts open(), abort() and timeouts. A transfer that settles for an older count has been
        // superseded and is ignored.
        uint32_t m_generation{0};

        uint32_t m_timeout{0};
        std::optional<double> m_timeoutId{};

        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;
        std::unordered_map<std::string, Napi::FunctionReference> m_onHandlers;
    };
}

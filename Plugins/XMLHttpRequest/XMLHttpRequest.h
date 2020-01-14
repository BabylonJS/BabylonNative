#pragma once

#include <napi/napi.h>
#include <unordered_map>
#include <Babylon/PluginHost.h>

namespace Babylon
{
    class PluginHost;

    enum class HTTPStatusCode : int32_t
    {
        None = 0,
        Continue = 100,
        SwitchingProtocols = 101,
        Processing = 102,
        Ok = 200,
        Created = 201,
        Accepted = 202,
        NonAuthoritativeInformation = 203,
        NoContent = 204,
        ResetContent = 205,
        PartialContent = 206,
        MultiStatus = 207,
        AlreadyReported = 208,
        IMUsed = 226,
        MultipleChoices = 300,
        MovedPermanently = 301,
        Found = 302,
        SeeOther = 303,
        NotModified = 304,
        UseProxy = 305,
        TemporaryRedirect = 307,
        PermanentRedirect = 308,
        BadRequest = 400,
        Unauthorized = 401,
        PaymentRequired = 402,
        Forbidden = 403,
        NotFound = 404,
        MethodNotAllowed = 405,
        NotAcceptable = 406,
        ProxyAuthenticationRequired = 407,
        RequestTimeout = 408,
        Conflict = 409,
        Gone = 410,
        LengthRequired = 411,
        PreconditionFailed = 412,
        RequestEntityTooLarge = 413,
        RequestUriTooLong = 414,
        UnsupportedMediaType = 415,
        RequestedRangeNotSatisfiable = 416,
        ExpectationFailed = 417,
        UnprocessableEntity = 422,
        Locked = 423,
        FailedDependency = 424,
        UpgradeRequired = 426,
        PreconditionRequired = 428,
        TooManyRequests = 429,
        RequestHeaderFieldsTooLarge = 431,
        InternalServerError = 500,
        NotImplemented = 501,
        BadGateway = 502,
        ServiceUnavailable = 503,
        GatewayTimeout = 504,
        HttpVersionNotSupported = 505,
        VariantAlsoNegotiates = 506,
        InsufficientStorage = 507,
        LoopDetected = 508,
        NotExtended = 510,
        NetworkAuthenticationRequired = 511,
    };

    namespace XMLHttpRequestTypes
    {
        namespace ResponseType
        {
            constexpr const char* Text = "text";
            constexpr const char* ArrayBuffer = "arraybuffer";
        }

        namespace EventType
        {
            constexpr const char* ReadyStateChange = "readystatechange";
            constexpr const char* Progress = "progress";
            constexpr const char* LoadEnd = "loadend";
        }
    }

    template<typename T> class XMLHttpRequestBase : public Napi::ObjectWrap<T>
    {
    public:
        static void CreateInstance(Napi::Env& env)
        {
            Napi::HandleScope scope{ env };

            Napi::Function func = DefineClass(
                env,
                "XMLHttpRequest",
                {
                    StaticValue("UNSENT", Napi::Value::From(env, 0)),
                    StaticValue("OPENED", Napi::Value::From(env, 1)),
                    StaticValue("HEADERS_RECEIVED", Napi::Value::From(env, 2)),
                    StaticValue("LOADING", Napi::Value::From(env, 3)),
                    StaticValue("DONE", Napi::Value::From(env, 4)),
                    InstanceAccessor("readyState", &T::GetReadyState, nullptr),
                    InstanceAccessor("response", &T::GetResponse, nullptr),
                    InstanceAccessor("responseText", &T::GetResponseText, nullptr),
                    InstanceAccessor("responseType", &T::GetResponseType, &T::SetResponseType),
                    InstanceAccessor("responseURL", &T::GetResponseURL, nullptr),
                    InstanceAccessor("status", &T::GetStatus, nullptr),
                    InstanceMethod("addEventListener", &T::AddEventListener),
                    InstanceMethod("removeEventListener", &T::RemoveEventListener),
                    InstanceMethod("open", &T::Open),
                    InstanceMethod("send", &T::Send),
                });

            constructor = Napi::Persistent(func);
            constructor.SuppressDestruct();

            env.Global().Set("XMLHttpRequest", func);
        }

        explicit XMLHttpRequestBase(const Napi::CallbackInfo& info)
            : Napi::ObjectWrap<T>{ info }
            , m_pluginHost{ PluginHost::GetRuntimeImplFromJavaScript(info.Env()) }
        {
        }

    protected:

        enum class ReadyState : int32_t
        {
            Unsent = 0,
            Opened = 1,
            Done = 4,
        };

        Napi::Value GetReadyState(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(Env(), static_cast<int32_t>(m_readyState));
        }

        Napi::Value GetResponse(const Napi::CallbackInfo& info)
        {
            return m_response.Value();
        }

        Napi::Value GetResponseText(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(Env(), m_responseText);
        }

        Napi::Value GetResponseType(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(Env(), m_responseType);
        }

        void SetResponseType(const Napi::CallbackInfo& info, const Napi::Value& value)
        {
            m_responseType = value.As<Napi::String>().Utf8Value();
        }

        Napi::Value GetResponseURL(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(Env(), m_responseURL);
        }

        Napi::Value GetStatus(const Napi::CallbackInfo& info)
        {
            return Napi::Value::From(Env(), static_cast<int32_t>(m_status));
        }

        void AddEventListener(const Napi::CallbackInfo& info)
        {
            std::string eventType = info[0].As<Napi::String>().Utf8Value().c_str();
            Napi::Function eventHandler = info[1].As<Napi::Function>();

            const auto& eventHandlerRefs = m_eventHandlerRefs[eventType];
            for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
            {
                if (it->Value() == eventHandler)
                {
                    throw Napi::Error::New(info.Env(), "Cannot add the same event handler twice");
                }
            }

            m_eventHandlerRefs[eventType].push_back(Napi::Persistent(eventHandler));
        }

        void RemoveEventListener(const Napi::CallbackInfo& info)
        {
            std::string eventType = info[0].As<Napi::String>().Utf8Value();
            Napi::Function eventHandler = info[1].As<Napi::Function>();
            auto itType = m_eventHandlerRefs.find(eventType);
            if (itType != m_eventHandlerRefs.end())
            {
                auto& eventHandlerRefs = itType->second;
                for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
                {
                    if (it->Value() == eventHandler)
                    {
                        eventHandlerRefs.erase(it);
                        break;
                    }
                }
            }
        }

        void Open(const Napi::CallbackInfo& info)
        {
            m_method = info[0].As<Napi::String>().Utf8Value();
            m_url = m_pluginHost.GetAbsoluteUrl(info[1].As<Napi::String>().Utf8Value());
            SetReadyState(ReadyState::Opened);
        }

        void Send(const Napi::CallbackInfo& info)
        {
            m_pluginHost.AddTask([this]() {
                SendAsync();
            });
        }
        
        virtual void SendAsync()
        {
            if (m_responseType.empty() || m_responseType == XMLHttpRequestTypes::ResponseType::Text)
            {
                m_pluginHost.LoadUrlAsync(m_url, [this](const std::string& data) {
                    m_responseText = std::move(data);
                    m_status = HTTPStatusCode::Ok;
                    m_pluginHost.AddTask([this]() {
                        SetReadyState(ReadyState::Done);
                    });
                });
            }
            else if (m_responseType == XMLHttpRequestTypes::ResponseType::ArrayBuffer)
            {
                m_pluginHost.LoadUrlAsync(m_url, [this](const std::vector<char>& data) {
                    m_response = Napi::Persistent(Napi::ArrayBuffer::New(Env(), data.size()));
                    memcpy(m_response.Value().Data(), data.data(), data.size());
                    m_status = HTTPStatusCode::Ok;
                    SetReadyState(ReadyState::Done);
                });
            }
            else
            {
                throw std::exception();
            }
        }
    
        void SetReadyState(ReadyState readyState)
        {
            m_readyState = readyState;

            auto it = m_eventHandlerRefs.find(XMLHttpRequestTypes::EventType::ReadyStateChange);
            if (it != m_eventHandlerRefs.end())
            {
                const auto& eventHandlerRefs = it->second;
                for (const auto& eventHandlerRef : eventHandlerRefs)
                {
                    eventHandlerRef.Call({});
                }
            }
        }

        PluginHost& m_pluginHost;

        ReadyState m_readyState{ReadyState::Unsent};
        Napi::Reference<Napi::ArrayBuffer> m_response;
        std::string m_responseText;
        std::string m_responseType;
        HTTPStatusCode m_status{HTTPStatusCode::None};
        std::string m_responseURL;
        std::unordered_map<std::string, std::vector<Napi::FunctionReference>> m_eventHandlerRefs;

        std::string m_method;
        std::string m_url;

        static inline Napi::FunctionReference constructor{};
    };

    class XMLHttpRequest : public XMLHttpRequestBase<XMLHttpRequest>
    {
    public:
        explicit XMLHttpRequest(const Napi::CallbackInfo& info)
            : XMLHttpRequestBase{ info }
        {
        }
        static void CreateInstance(Napi::Env env)
        {
            XMLHttpRequestBase<XMLHttpRequest>::CreateInstance(env);
        }
    };
}

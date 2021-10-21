#include "XMLHttpRequest.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <sstream>

bool IsHexChar(const char& c)
{
    return ((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') || (c >= '0' && c <= '9'));
}

std::string EncodePercent(const std::string& input)
{
    std::ostringstream encoded;
    for (auto i = input.begin(), e = input.end(); i != e; ++i)
    {
        encoded << *i;
        if (*i == '%')
        {
            if (std::distance(i, e) >= 2 && !(IsHexChar(*(i + 1)) && IsHexChar(*(i + 2))))
            {
                // If a percent character is not followed by two hex characters, we should encode it
                encoded << "25";
            }
        }
    }
    return encoded.str();
}

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        namespace ResponseType
        {
            constexpr const char* Text = "text";
            constexpr const char* ArrayBuffer = "arraybuffer";

            UrlLib::UrlResponseType StringToEnum(const std::string& value)
            {
                if (value == Text)
                    return UrlLib::UrlResponseType::String;
                if (value == ArrayBuffer)
                    return UrlLib::UrlResponseType::Buffer;

                throw std::runtime_error{"Unsupported response type: " + value};
            }

            const char* EnumToString(UrlLib::UrlResponseType value)
            {
                switch (value)
                {
                    case UrlLib::UrlResponseType::String:
                        return Text;
                    case UrlLib::UrlResponseType::Buffer:
                        return ArrayBuffer;
                }

                throw std::runtime_error{"Invalid response type"};
            }
        }

        namespace MethodType
        {
            constexpr const char* Get = "GET";

            UrlLib::UrlMethod StringToEnum(const std::string& value)
            {
                if (value == Get)
                    return UrlLib::UrlMethod::Get;

                throw std::runtime_error{"Unsupported url method: " + value};
            }
        }

        namespace EventType
        {
            constexpr const char* ReadyStateChange = "readystatechange";
            constexpr const char* LoadEnd = "loadend";
        }
    }

    void XMLHttpRequest::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        static constexpr auto JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME = "XMLHttpRequest";

        Napi::Function func = DefineClass(
            env,
            JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME,
            {
                StaticValue("UNSENT", Napi::Value::From(env, 0)),
                StaticValue("OPENED", Napi::Value::From(env, 1)),
                StaticValue("HEADERS_RECEIVED", Napi::Value::From(env, 2)),
                StaticValue("LOADING", Napi::Value::From(env, 3)),
                StaticValue("DONE", Napi::Value::From(env, 4)),
                InstanceAccessor("readyState", &XMLHttpRequest::GetReadyState, nullptr),
                InstanceAccessor("response", &XMLHttpRequest::GetResponse, nullptr),
                InstanceAccessor("responseText", &XMLHttpRequest::GetResponseText, nullptr),
                InstanceAccessor("responseType", &XMLHttpRequest::GetResponseType, &XMLHttpRequest::SetResponseType),
                InstanceAccessor("responseURL", &XMLHttpRequest::GetResponseURL, nullptr),
                InstanceAccessor("status", &XMLHttpRequest::GetStatus, nullptr),
                InstanceMethod("addEventListener", &XMLHttpRequest::AddEventListener),
                InstanceMethod("removeEventListener", &XMLHttpRequest::RemoveEventListener),
                InstanceMethod("abort", &XMLHttpRequest::Abort),
                InstanceMethod("open", &XMLHttpRequest::Open),
                InstanceMethod("send", &XMLHttpRequest::Send),
            });

        if (env.Global().Get(JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME).IsUndefined())
        {
            env.Global().Set(JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME, func);
        }

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_XML_HTTP_REQUEST_CONSTRUCTOR_NAME, func);
    }

    XMLHttpRequest::XMLHttpRequest(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<XMLHttpRequest>{info}
        , m_runtimeScheduler{JsRuntime::GetFromJavaScript(info.Env())}
    {
    }

    Napi::Value XMLHttpRequest::GetReadyState(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), arcana::underlying_cast(m_readyState));
    }

    Napi::Value XMLHttpRequest::GetResponse(const Napi::CallbackInfo&)
    {
        gsl::span<const std::byte> responseBuffer{m_request.ResponseBuffer()};
        auto arrayBuffer{Napi::ArrayBuffer::New(Env(), responseBuffer.size())};
        std::memcpy(arrayBuffer.Data(), responseBuffer.data(), arrayBuffer.ByteLength());
        return std::move(arrayBuffer);
    }

    Napi::Value XMLHttpRequest::GetResponseText(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_request.ResponseString().data());
    }

    Napi::Value XMLHttpRequest::GetResponseType(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), ResponseType::EnumToString(m_request.ResponseType()));
    }

    void XMLHttpRequest::SetResponseType(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        m_request.ResponseType(ResponseType::StringToEnum(value.As<Napi::String>().Utf8Value()));
    }

    Napi::Value XMLHttpRequest::GetResponseURL(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_request.ResponseUrl().data());
    }

    Napi::Value XMLHttpRequest::GetStatus(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), arcana::underlying_cast(m_request.StatusCode()));
    }

    void XMLHttpRequest::AddEventListener(const Napi::CallbackInfo& info)
    {
        std::string eventType = info[0].As<Napi::String>().Utf8Value();
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

    void XMLHttpRequest::RemoveEventListener(const Napi::CallbackInfo& info)
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

    void XMLHttpRequest::Abort(const Napi::CallbackInfo&)
    {
        m_request.Abort();
    }

    void XMLHttpRequest::Open(const Napi::CallbackInfo& info)
    {
        try
        {
            // printfs for debugging CI, will be removed
            auto inputURL{info[1].As<Napi::String>()};
            // If the input URL contains any true % characters, encode them as %25
            auto encodedPercentURL{Napi::String::New(info.Env(), EncodePercent(inputURL.Utf8Value()))};
            // Decode the input URL to get a completely unencoded URL
            auto decodedURL{info.Env().Global().Get("decodeURI").As<Napi::Function>().Call({encodedPercentURL})};
            // Re-encode the URL to make sure that every illegal character is encoded
            auto finalURL{info.Env().Global().Get("encodeURI").As<Napi::Function>().Call({decodedURL}).As<Napi::String>()};
            m_request.Open(MethodType::StringToEnum(info[0].As<Napi::String>().Utf8Value()), finalURL.Utf8Value());
            SetReadyState(ReadyState::Opened);
        }
        catch (const std::exception& e)
        {
            // If we have a parse error, catch and rethrow to JavaScript
            throw Napi::Error::New(info.Env(), std::string{"Error parsing URL scheme: "} + e.what());
        }
        catch (...)
        {
            throw Napi::Error::New(info.Env(), "Unknown error parsing URL scheme");
        }
    }

    void XMLHttpRequest::Send(const Napi::CallbackInfo& info)
    {
        if (m_readyState != ReadyState::Opened)
        {
            throw Napi::Error::New(info.Env(), "XMLHttpRequest must be opened before it can be sent");
            return;
        }
        m_request.SendAsync().then(m_runtimeScheduler, arcana::cancellation::none(), [env{info.Env()}, this](arcana::expected<void, std::exception_ptr> result) {
            if (result.has_error())
            {
                Napi::Error::New(env, result.error()).ThrowAsJavaScriptException();
                return;
            }

            SetReadyState(ReadyState::Done);
            RaiseEvent(EventType::LoadEnd);

            // Assume the XMLHttpRequest will only be used for a single request and clear the event handlers.
            // Single use seems to be the standard pattern, and we need to release our strong refs to event handlers.
            m_eventHandlerRefs.clear();
        });
    }

    void XMLHttpRequest::SetReadyState(ReadyState readyState)
    {
        m_readyState = readyState;
        RaiseEvent(EventType::ReadyStateChange);
    }

    void XMLHttpRequest::RaiseEvent(const char* eventType)
    {
        auto it = m_eventHandlerRefs.find(eventType);
        if (it != m_eventHandlerRefs.end())
        {
            const auto& eventHandlerRefs = it->second;
            for (const auto& eventHandlerRef : eventHandlerRefs)
            {
                eventHandlerRef.Call({});
            }
        }
    }
}

namespace Babylon::Polyfills::XMLHttpRequest
{
    void Initialize(Napi::Env env)
    {
        Internal::XMLHttpRequest::Initialize(env);
    }
}

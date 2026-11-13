#include "XMLHttpRequest.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/XMLHttpRequest.h>
#include <arcana/tracing/trace_region.h>
#include <algorithm>
#include <cstring>
#include <memory>
#include <sstream>
#include <string_view>
#include <utility>

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
            constexpr const char* Post = "POST";

            UrlLib::UrlMethod StringToEnum(const std::string& value)
            {
                if (value == Get)
                    return UrlLib::UrlMethod::Get;
                else if (value == Post)
                    return UrlLib::UrlMethod::Post;

                throw std::runtime_error{"Unsupported url method: " + value};
            }
        }

        namespace EventType
        {
            constexpr const char* ReadyStateChange = "readystatechange";
            constexpr const char* LoadStart = "loadstart";
            constexpr const char* Progress = "progress";
            constexpr const char* Load = "load";
            constexpr const char* LoadEnd = "loadend";
            constexpr const char* Error = "error";
            constexpr const char* Abort = "abort";
            constexpr const char* Timeout = "timeout";
        }

        char ToUpper(char c)
        {
            return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
        }

        bool IsHttpUrl(std::string_view url)
        {
            const auto hasScheme = [url](std::string_view scheme) {
                return url.size() > scheme.size() && url[scheme.size()] == ':' &&
                    std::equal(scheme.begin(), scheme.end(), url.begin(), [](char l, char r) { return ToUpper(l) == ToUpper(r); });
            };
            return hasScheme("http") || hasScheme("https");
        }

        // The `total` of the progress events of a response whose body is `loaded` bytes long: the
        // Content-Length the server announced, or 0 when the length is not known. A response with
        // a Content-Encoding announces the encoded length while `loaded` counts decoded bytes, so
        // its length counts as unknown, as it does in browsers. Local files have no headers, but
        // their length is known.
        double TotalLength(const UrlLib::UrlRequest& request, std::string_view url, double loaded)
        {
            if (!IsHttpUrl(url))
            {
                return loaded;
            }

            if (request.GetResponseHeader("content-encoding").has_value())
            {
                return 0;
            }

            const auto contentLength = request.GetResponseHeader("content-length");
            if (!contentLength.has_value() || contentLength->empty())
            {
                return 0;
            }

            double total = 0;
            for (const char c : *contentLength)
            {
                if (c < '0' || c > '9')
                {
                    return 0;
                }
                total = total * 10 + (c - '0');
            }
            return total;
        }
    }

    void XMLHttpRequest::Initialize(Napi::Env env)
    {
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
                InstanceValue("UNSENT", Napi::Value::From(env, 0)),
                InstanceValue("OPENED", Napi::Value::From(env, 1)),
                InstanceValue("HEADERS_RECEIVED", Napi::Value::From(env, 2)),
                InstanceValue("LOADING", Napi::Value::From(env, 3)),
                InstanceValue("DONE", Napi::Value::From(env, 4)),
                InstanceAccessor("readyState", &XMLHttpRequest::GetReadyState, nullptr),
                InstanceAccessor("response", &XMLHttpRequest::GetResponse, nullptr),
                InstanceAccessor("responseText", &XMLHttpRequest::GetResponseText, nullptr),
                InstanceAccessor("responseType", &XMLHttpRequest::GetResponseType, &XMLHttpRequest::SetResponseType),
                InstanceAccessor("responseURL", &XMLHttpRequest::GetResponseURL, nullptr),
                InstanceAccessor("status", &XMLHttpRequest::GetStatus, nullptr),
                InstanceAccessor("statusText", &XMLHttpRequest::GetStatusText, nullptr),
                InstanceAccessor("timeout", &XMLHttpRequest::GetTimeout, &XMLHttpRequest::SetTimeout),
                InstanceAccessor("onreadystatechange", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::ReadyStateChange)),
                InstanceAccessor("onloadstart", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::LoadStart)),
                InstanceAccessor("onprogress", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::Progress)),
                InstanceAccessor("onload", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::Load)),
                InstanceAccessor("onloadend", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::LoadEnd)),
                InstanceAccessor("onerror", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::Error)),
                InstanceAccessor("onabort", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::Abort)),
                InstanceAccessor("ontimeout", &XMLHttpRequest::GetOnHandler, &XMLHttpRequest::SetOnHandler, napi_default, const_cast<char*>(EventType::Timeout)),
                // Non-standard, additive diagnostics: the normalized transport-error detail from
                // UrlLib, empty unless the request failed at the transport layer. Browsers do not
                // expose these, so spec-conformant code is unaffected; BN-aware code can read them
                // to tell a DNS failure from a refused connection or a missing local asset.
                InstanceAccessor("errorCode", &XMLHttpRequest::GetErrorCode, nullptr),
                InstanceAccessor("errorDetail", &XMLHttpRequest::GetErrorDetail, nullptr),
                InstanceMethod("getAllResponseHeaders", &XMLHttpRequest::GetAllResponseHeaders),
                InstanceMethod("getResponseHeader", &XMLHttpRequest::GetResponseHeader),
                InstanceMethod("setRequestHeader", &XMLHttpRequest::SetRequestHeader),
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
        if (m_request.ResponseType() == UrlLib::UrlResponseType::String)
        {
            const std::string_view responseString{m_hasResponse ? m_request.ResponseString() : std::string_view{}};
            return Napi::String::New(Env(), responseString.data(), responseString.size());
        }
        else
        {
            gsl::span<const std::byte> responseBuffer{};
            if (m_hasResponse)
            {
                responseBuffer = m_request.ResponseBuffer();
            }
            auto arrayBuffer{Napi::ArrayBuffer::New(Env(), responseBuffer.size())};
            if (!responseBuffer.empty())
            {
                std::memcpy(arrayBuffer.Data(), responseBuffer.data(), arrayBuffer.ByteLength());
            }
            return arrayBuffer;
        }
    }

    Napi::Value XMLHttpRequest::GetResponseText(const Napi::CallbackInfo&)
    {
        // The body may legitimately contain embedded nulls: Emscripten's EXPORT_ES6 output, for
        // example, inlines the .wasm payload as a JavaScript string literal. Passing .data()
        // alone would hand a const char* to Napi and truncate at the first null, so the length
        // has to be supplied explicitly.
        const std::string_view responseString{m_hasResponse ? m_request.ResponseString() : std::string_view{}};
        return Napi::String::New(Env(), responseString.data(), responseString.size());
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
        // The URL of the response after redirects, without a fragment; "" while there is no
        // response and after a network error.
        return Napi::String::New(Env(), m_hasResponse ? std::string{m_request.ResponseUrl()} : std::string{});
    }

    Napi::Value XMLHttpRequest::GetStatus(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_hasResponse ? arcana::underlying_cast(m_request.StatusCode()) : 0);
    }

    Napi::Value XMLHttpRequest::GetStatusText(const Napi::CallbackInfo&)
    {
        // Per the XHR spec, statusText is the empty string until a response is available
        // (status 0 means UNSENT/OPENED or a network error).
        return Napi::String::New(Env(), m_hasResponse ? std::string{m_request.StatusText()} : std::string{});
    }

    Napi::Value XMLHttpRequest::GetErrorCode(const Napi::CallbackInfo&)
    {
        // Stable symbolic token for a transport failure (e.g. "CURLE_COULDNT_CONNECT",
        // "NSURLErrorTimedOut", "AppResourceNotFound"); empty when there was no transport failure.
        return Napi::String::New(Env(), std::string{m_request.ErrorSymbol()});
    }

    Napi::Value XMLHttpRequest::GetErrorDetail(const Napi::CallbackInfo&)
    {
        // Full normalized "<domain>:<symbol>(<code>): <detail>" string; empty when there was no
        // transport failure.
        return Napi::String::New(Env(), std::string{m_request.ErrorString()});
    }

    Napi::Value XMLHttpRequest::GetTimeout(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_timeout);
    }

    void XMLHttpRequest::SetTimeout(const Napi::CallbackInfo&, const Napi::Value& value)
    {
        // Milliseconds; 0 means no timeout. It applies to the next send().
        m_timeout = value.IsNumber() ? value.As<Napi::Number>().Uint32Value() : 0;
    }

    Napi::Value XMLHttpRequest::GetOnHandler(const Napi::CallbackInfo& info)
    {
        const auto it = m_onHandlers.find(static_cast<const char*>(info.Data()));
        if (it != m_onHandlers.end() && !it->second.IsEmpty())
        {
            return it->second.Value();
        }
        return info.Env().Null();
    }

    void XMLHttpRequest::SetOnHandler(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const auto* eventType = static_cast<const char*>(info.Data());
        if (value.IsFunction())
        {
            m_onHandlers[eventType] = Napi::Persistent(value.As<Napi::Function>());
        }
        else
        {
            // Assigning null (or anything that is not a function) clears the handler.
            m_onHandlers.erase(eventType);
        }
    }

    Napi::Value XMLHttpRequest::GetResponseHeader(const Napi::CallbackInfo& info)
    {
        if (!m_hasResponse)
        {
            return info.Env().Null();
        }

        const auto headerName = info[0].As<Napi::String>().Utf8Value();
        const auto header = m_request.GetResponseHeader(headerName);
        return header ? Napi::Value::From(Env(), header.value()) : info.Env().Null();
    }

    Napi::Value XMLHttpRequest::GetAllResponseHeaders(const Napi::CallbackInfo&)
    {
        // The XMLHttpRequest standard returns one "name: value\r\n" line per header, with the
        // names lower-cased and the lines sorted by the byte order of the upper-cased names (so
        // "ewok" sorts before "__custom"). UrlLib has already lower-cased the names and combined
        // the values of a repeated header with ", ".
        std::string serialized{};
        if (m_hasResponse)
        {
            const auto& responseHeaders = m_request.GetAllResponseHeaders();
            std::vector<std::pair<std::string, const std::pair<const std::string, std::string>*>> headers{};
            headers.reserve(responseHeaders.size());
            for (const auto& header : responseHeaders)
            {
                std::string sortKey{header.first};
                std::transform(sortKey.begin(), sortKey.end(), sortKey.begin(), ToUpper);
                headers.emplace_back(std::move(sortKey), &header);
            }
            std::sort(headers.begin(), headers.end(), [](const auto& l, const auto& r) { return l.first < r.first; });

            for (const auto& header : headers)
            {
                serialized += header.second->first;
                serialized += ": ";
                serialized += header.second->second;
                serialized += "\r\n";
            }
        }

        return Napi::String::New(Env(), serialized);
    }

    void XMLHttpRequest::SetRequestHeader(const Napi::CallbackInfo& info)
    {
        m_request.SetRequestHeader(info[0].As<Napi::String>().Utf8Value(), info[1].As<Napi::String>().Utf8Value());
    }

    void XMLHttpRequest::AddEventListener(const Napi::CallbackInfo& info)
    {
        const std::string eventType = info[0].As<Napi::String>().Utf8Value();
        const Napi::Function eventHandler = info[1].As<Napi::Function>();

        // Adding a listener that is already registered for the event has no effect.
        auto& eventHandlerRefs = m_eventHandlerRefs[eventType];
        for (auto it = eventHandlerRefs.begin(); it != eventHandlerRefs.end(); ++it)
        {
            if (it->Value() == eventHandler)
            {
                return;
            }
        }

        eventHandlerRefs.push_back(Napi::Persistent(eventHandler));
    }

    void XMLHttpRequest::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        const std::string eventType = info[0].As<Napi::String>().Utf8Value();
        const Napi::Function eventHandler = info[1].As<Napi::Function>();
        const auto itType = m_eventHandlerRefs.find(eventType);
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

    bool XMLHttpRequest::IsInFlight() const
    {
        return (m_readyState == ReadyState::Opened && m_sendFlag) ||
            m_readyState == ReadyState::HeadersReceived ||
            m_readyState == ReadyState::Loading;
    }

    void XMLHttpRequest::Supersede()
    {
        ++m_generation;
        CancelTimeout();
        if (m_requestSent)
        {
            // Stop the transfer if it is still running. Its completion is ignored either way,
            // because it carries the previous generation.
            m_request.Abort();
        }
    }

    void XMLHttpRequest::Abort(const Napi::CallbackInfo& info)
    {
        const Napi::Object self{info.This().As<Napi::Object>()};

        // The abort() steps of the XMLHttpRequest standard: a request that is under way ends with
        // readystatechange (DONE), abort and loadend, raised before abort() returns. The request is
        // then UNSENT again, without another readystatechange.
        if (IsInFlight())
        {
            Supersede();
            const uint32_t generation{m_generation};
            RequestErrorSteps(self, EventType::Abort);
            if (generation != m_generation)
            {
                // One of the handlers opened a new request.
                return;
            }
            ReleaseHandlers(generation);
        }

        if (m_readyState == ReadyState::Done)
        {
            m_readyState = ReadyState::Unsent;
            m_hasResponse = false;
        }
    }

    void XMLHttpRequest::Open(const Napi::CallbackInfo& info)
    {
        m_url = info[1].As<Napi::String>();

        // open() abandons whatever the previous open() started.
        Supersede();
        if (m_requestSent)
        {
            // A UrlRequest that was sent may still be completing on another thread, and one that
            // was aborted stays cancelled, so start over with a new one. The transfer that is under
            // way keeps the previous one alive until it settles.
            const auto responseType = m_request.ResponseType();
            m_request = UrlLib::UrlRequest{};
            m_request.ResponseType(responseType);
            m_requestSent = false;
        }
        m_sendFlag = false;
        m_hasResponse = false;

        try
        {
            m_request.Open(MethodType::StringToEnum(info[0].As<Napi::String>().Utf8Value()), m_url);
        }
        catch (const std::exception& e)
        {
            throw Napi::Error::New(info.Env(), std::string{"Error opening URL: "} + e.what());
        }
        catch (...)
        {
            throw Napi::Error::New(info.Env(), "Unknown error opening URL");
        }

        if (m_readyState != ReadyState::Opened)
        {
            SetReadyState(info.This().As<Napi::Object>(), ReadyState::Opened);
        }
    }

    void XMLHttpRequest::Send(const Napi::CallbackInfo& info)
    {
        if (m_readyState != ReadyState::Opened)
        {
            throw Napi::Error::New(info.Env(), "XMLHttpRequest must be opened before it can be sent");
        }

        if (m_sendFlag)
        {
            throw Napi::Error::New(info.Env(), "XMLHttpRequest has already been sent");
        }

        if (info.Length() > 0)
        {
            if (!info[0].IsString() && !info[0].IsUndefined() && !info[0].IsNull())
            {
                throw Napi::Error::New(info.Env(), "Only strings are supported in XMLHttpRequest body");
            }

            if (info[0].IsString())
            {
                m_request.SetRequestBody(info[0].As<Napi::String>().Utf8Value());
            }
        }

        const Napi::Object self{info.This().As<Napi::Object>()};
        const uint32_t generation{m_generation};

        m_sendFlag = true;
        RaiseEvent(self, EventType::LoadStart);
        if (generation != m_generation || m_readyState != ReadyState::Opened || !m_sendFlag)
        {
            // A loadstart handler aborted the request or opened a new one.
            return;
        }

        std::string traceName = (std::ostringstream{} << "XMLHttpRequest::Send [" << m_url << "]").str();
        auto sendRegion = std::make_optional<arcana::trace_region>(traceName.c_str());

        // Keep the JS wrapper (and therefore this C++ object) alive for the
        // duration of the asynchronous request. The continuation below captures
        // `this` raw and dereferences members when the request settles; without
        // an anchor, GC may collect the wrapper while the request is in flight
        // (e.g. once the requesting script drops its reference) and the
        // continuation would then run on a freed `this`. The anchor lives in a
        // shared_ptr owned by the continuation lambda, so it is released
        // automatically once the request settles and the lambda is destroyed --
        // no member self-reference to clear. (Mirrors FileReader's anchor.)
        auto anchor = std::make_shared<Napi::ObjectReference>(Napi::Persistent(self));

        m_requestSent = true;
        StartTimeout(self);

        try
        {
            m_request.SendAsync()
                .then(arcana::inline_scheduler, arcana::cancellation::none(), [sendRegion{std::move(sendRegion)}]() mutable {
                    sendRegion.reset();
                })
                .then(m_runtimeScheduler, arcana::cancellation::none(), [this, anchor{std::move(anchor)}, generation, request{m_request}](const arcana::expected<void, std::exception_ptr>& result) {
                    // `request` is the UrlRequest this transfer was started on. Holding it here keeps
                    // it alive until the platform is done with it, also when abort() or open() has
                    // moved m_request on to a new one in the meantime.
                    static_cast<void>(request);

                    // Run on every outcome -- a transport exception as well as a completed transfer --
                    // so the request always reaches DONE and loadend.
                    OnSettled(anchor->Value(), generation, result.has_error());
                });
        }
        catch (...)
        {
            // The transfer could not be started, so there is nothing to wait for.
            m_sendFlag = false;
            CancelTimeout();
            throw;
        }
    }

    void XMLHttpRequest::OnSettled(const Napi::Object& self, uint32_t generation, bool transportFailed)
    {
        if (generation != m_generation)
        {
            // abort(), a timeout or a new open() got there first and has raised its own events.
            return;
        }

        CancelTimeout();

        // A network error: the transport failed, or it finished without a response (UrlLib leaves
        // the status at 0, e.g. for a missing local file). Any HTTP status, including 404 and 500,
        // is a response and ends with load, not error.
        if (transportFailed || arcana::underlying_cast(m_request.StatusCode()) == 0)
        {
            RequestErrorSteps(self, EventType::Error);
            ReleaseHandlers(generation);
            return;
        }

        // UrlLib delivers the response in one piece, so the states and events of receiving it are
        // raised back to back, in the order of the XMLHttpRequest standard: HEADERS_RECEIVED,
        // LOADING if there is a body, one progress event, DONE, load, loadend.
        m_hasResponse = true;
        const double loaded{static_cast<double>(m_request.ResponseType() == UrlLib::UrlResponseType::String ? m_request.ResponseString().size() : m_request.ResponseBuffer().size())};
        const double total{TotalLength(m_request, m_url, loaded)};

        // A handler that calls abort() or open() ends this request, which shows as a new generation.
        SetReadyState(self, ReadyState::HeadersReceived);
        if (generation != m_generation)
        {
            return;
        }

        if (loaded > 0)
        {
            SetReadyState(self, ReadyState::Loading);
            if (generation != m_generation)
            {
                return;
            }
        }

        RaiseEvent(self, EventType::Progress, loaded, total);
        if (generation != m_generation)
        {
            return;
        }

        m_sendFlag = false;
        SetReadyState(self, ReadyState::Done);
        if (generation != m_generation)
        {
            return;
        }

        RaiseEvent(self, EventType::Load, loaded, total);
        if (generation != m_generation)
        {
            return;
        }

        RaiseEvent(self, EventType::LoadEnd, loaded, total);
        ReleaseHandlers(generation);
    }

    void XMLHttpRequest::RequestErrorSteps(const Napi::Object& self, const char* eventType)
    {
        const uint32_t generation{m_generation};

        m_sendFlag = false;
        m_hasResponse = false;
        SetReadyState(self, ReadyState::Done);
        if (generation != m_generation)
        {
            return;
        }

        RaiseEvent(self, eventType);
        if (generation != m_generation)
        {
            return;
        }

        RaiseEvent(self, EventType::LoadEnd);
    }

    void XMLHttpRequest::StartTimeout(const Napi::Object& self)
    {
        if (m_timeout == 0)
        {
            return;
        }

        // The timer comes from the host's setTimeout; without one there is no timeout.
        Napi::Env env{Env()};
        const Napi::Value setTimeout{env.Global().Get("setTimeout")};
        if (!setTimeout.IsFunction())
        {
            return;
        }

        // The callback keeps the JS wrapper alive, like the anchor in Send(), for as long as the
        // timer holds on to it.
        auto anchor = std::make_shared<Napi::ObjectReference>(Napi::Persistent(self));
        const uint32_t generation{m_generation};
        const Napi::Function callback = Napi::Function::New(env, [this, anchor{std::move(anchor)}, generation](const Napi::CallbackInfo&) {
            if (generation != m_generation || !IsInFlight())
            {
                return;
            }

            m_timeoutId.reset();
            Supersede();
            const uint32_t timedOutGeneration{m_generation};
            RequestErrorSteps(anchor->Value(), EventType::Timeout);
            ReleaseHandlers(timedOutGeneration);
        });

        const Napi::Value timeoutId{setTimeout.As<Napi::Function>().Call({callback, Napi::Value::From(env, m_timeout)})};
        if (timeoutId.IsNumber())
        {
            m_timeoutId = timeoutId.As<Napi::Number>().DoubleValue();
        }
    }

    void XMLHttpRequest::CancelTimeout()
    {
        if (!m_timeoutId.has_value())
        {
            return;
        }

        Napi::Env env{Env()};
        const Napi::Value clearTimeout{env.Global().Get("clearTimeout")};
        if (clearTimeout.IsFunction())
        {
            clearTimeout.As<Napi::Function>().Call({Napi::Value::From(env, *m_timeoutId)});
        }
        m_timeoutId.reset();
    }

    void XMLHttpRequest::ReleaseHandlers(uint32_t generation)
    {
        if (generation != m_generation)
        {
            // A handler opened a new request; the handlers now belong to it.
            return;
        }

        // Assume the XMLHttpRequest will only be used for a single request and clear the event handlers.
        // Single use seems to be the standard pattern, and we need to release our strong refs to event handlers.
        m_eventHandlerRefs.clear();
        m_onHandlers.clear();
    }

    void XMLHttpRequest::SetReadyState(const Napi::Object& self, ReadyState readyState)
    {
        m_readyState = readyState;
        RaiseEvent(self, EventType::ReadyStateChange);
    }

    void XMLHttpRequest::RaiseEvent(const Napi::Object& self, const char* eventType, double loaded, double total)
    {
        std::string traceName = (std::ostringstream{} << "XMLHttpRequest::RaiseEvent [" << eventType << "] [" << m_url << "]").str();
        arcana::trace_region raiseEventRegion{traceName.c_str()};

        // Take a snapshot: a handler may add or remove listeners, or end the request, which
        // releases all of them. The on<event> handler runs before the listeners.
        std::vector<Napi::Function> handlers{};
        const auto onHandler = m_onHandlers.find(eventType);
        if (onHandler != m_onHandlers.end() && !onHandler->second.IsEmpty())
        {
            handlers.push_back(onHandler->second.Value());
        }
        const auto listeners = m_eventHandlerRefs.find(eventType);
        if (listeners != m_eventHandlerRefs.end())
        {
            for (const auto& eventHandlerRef : listeners->second)
            {
                handlers.push_back(eventHandlerRef.Value());
            }
        }
        if (handlers.empty())
        {
            return;
        }

        // A plain object with the members of a ProgressEvent. For readystatechange, which is not a
        // progress event, loaded and total are 0.
        Napi::Env env{Env()};
        Napi::Object event = Napi::Object::New(env);
        event.Set("type", Napi::String::New(env, eventType));
        event.Set("target", self);
        event.Set("currentTarget", self);
        event.Set("lengthComputable", Napi::Boolean::New(env, total != 0));
        event.Set("loaded", Napi::Number::New(env, loaded));
        event.Set("total", Napi::Number::New(env, total));

        for (const auto& handler : handlers)
        {
            // As in a browser, an exception thrown by a handler does not reach the caller of open(),
            // send() or abort() and does not keep the remaining handlers and events from running.
            try
            {
                handler.Call(self, {event});
            }
            catch (const Napi::Error&)
            {
            }
        }
    }
}

namespace Babylon::Polyfills::XMLHttpRequest
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::XMLHttpRequest::Initialize(env);
    }
}

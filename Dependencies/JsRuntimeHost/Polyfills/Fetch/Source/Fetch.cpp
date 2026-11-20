#include "Fetch.h"
#include "FetchScripts.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Polyfills/Fetch.h>

#include <UrlLib/UrlLib.h>
#include <Babylon/Polyfills/DataUrl.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    using DataUrlResponse = Babylon::Polyfills::DataUrl::Response;
    namespace
    {
        // Shared state for honoring an AbortSignal passed via init.signal. Co-owned by the "abort"
        // listener (which sets the flag, captures the reason, and cancels the transport) and the
        // completion continuation (which reports the AbortError and tears the listener down).
        struct AbortState
        {
            bool aborted{false};
            Napi::ObjectReference reasonHolder;
            Napi::ObjectReference signal;
            Napi::FunctionReference listener;
        };

        // The reason a fetch was aborted: the signal's `reason` (per the modern AbortSignal), or a
        // fresh AbortError if the signal does not expose one.
        Napi::Value GetAbortReason(Napi::Env env, const Napi::Object& signal)
        {
            const Napi::Value reason = signal.Get("reason");
            if (!reason.IsUndefined() && !reason.IsNull())
            {
                return reason;
            }

            Napi::Error error = Napi::Error::New(env, "The operation was aborted.");
            error.Set("name", Napi::String::New(env, "AbortError"));
            return error.Value();
        }

        bool EqualsIgnoreCase(std::string_view a, std::string_view b)
        {
            return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](unsigned char l, unsigned char r) {
                return std::tolower(l) == std::tolower(r);
            });
        }


        // Stable message used for every transport-failure rejection. Browsers and Node both keep
        // this constant (the variable detail rides on `cause`) so crash-report grouping stays
        // intact; we follow Node/undici's "fetch failed" spelling.
        constexpr const char* FETCH_FAILED_MESSAGE = "fetch failed";
        constexpr const char* JS_FETCH_POLYFILL_EXPORTS_NAME = "fetchPolyfillExports";

        // Snapshot the JS call-site stack synchronously, inside fetch(), before SendAsync() hands
        // the request to a worker thread. The transport-failure rejection is otherwise built in a
        // continuation that runs after fetch() has returned, where an Error would capture zero user
        // frames. We go through the global JS `Error` constructor (rather than napi_create_error) so
        // engines that materialize `.stack` from the JS constructor path capture the live caller
        // frames. The result is a plain std::string, safe to carry across the thread hop (unlike a
        // Napi::Reference, which must be created and destroyed on the JS thread). Empty if the engine
        // does not expose a stack at construction time (e.g. Chakra, which only populates `.stack`
        // when an error is thrown) -- in that case the rejection simply carries no synthetic frames.
        std::string CaptureCallSiteStack(Napi::Env env)
        {
            // Error is always present and callable; this guard is defensive.
            const Napi::Value errorCtor = env.Global().Get("Error");
            if (!errorCtor.IsFunction())
            {
                return {};
            }
            const Napi::Object error = errorCtor.As<Napi::Function>().New({});
            const Napi::Value stack = error.Get("stack");
            return stack.IsString() ? stack.As<Napi::String>().Utf8Value() : std::string{};
        }

        // Reattach the synchronously-captured frames to the rejection's Error, replacing the captured
        // header line (e.g. "Error\n    at ...") with one matching the TypeError we actually reject
        // with, so the stack reads correctly while preserving the user's call site.
        std::string ComposeRejectionStack(const std::string& capturedStack)
        {
            std::string header{"TypeError: "};
            header += FETCH_FAILED_MESSAGE;

            const auto firstNewline = capturedStack.find('\n');
            if (firstNewline == std::string::npos)
            {
                return header;
            }
            return header + capturedStack.substr(firstNewline);
        }

        // Build the transport-failure rejection: a TypeError with a stable message, carrying the
        // variable detail under `cause` (Node/undici shape) rather than as top-level own-properties.
        // `code`/`detail` come from UrlLib's normalized accessors and may be empty on backends that
        // do not yet populate them (Windows/Android today) -- in that case the standard observable
        // shape (TypeError + stable message + url) is preserved and only the extra detail is absent.
        Napi::Error BuildTransportError(Napi::Env env, const UrlLib::UrlRequest& request, const std::string& url, const std::string& capturedStack)
        {
            Napi::Error error = Napi::TypeError::New(env, FETCH_FAILED_MESSAGE);

            Napi::Object cause = Napi::Object::New(env);
            const std::string code{request.ErrorSymbol()};
            const std::string detail{request.ErrorString()};
            if (!code.empty())
            {
                cause.Set("code", Napi::String::New(env, code));
            }
            if (!detail.empty())
            {
                cause.Set("detail", Napi::String::New(env, detail));
            }
            cause.Set("url", Napi::String::New(env, url));
            cause.Set("status", Napi::Number::New(env, static_cast<double>(static_cast<int>(request.StatusCode()))));
            error.Set("cause", cause);

            if (!capturedStack.empty())
            {
                error.Set("stack", Napi::String::New(env, ComposeRejectionStack(capturedStack)));
            }

            return error;
        }

        // fetch only resolves for GET and POST because the underlying UrlLib transport supports nothing else.
        UrlLib::UrlMethod ParseMethod(const std::string& method)
        {
            if (EqualsIgnoreCase(method, "GET"))
            {
                return UrlLib::UrlMethod::Get;
            }
            if (EqualsIgnoreCase(method, "POST"))
            {
                return UrlLib::UrlMethod::Post;
            }

            throw std::runtime_error{"Unsupported fetch method: " + method + " (only GET and POST are supported)"};
        }

        void ApplyRequestHeaders(UrlLib::UrlRequest& request, const Napi::Value& headers)
        {
            if (headers.IsUndefined() || headers.IsNull())
            {
                return;
            }

            const auto env = headers.Env();
            const auto headersConstructor = env.Global().Get("Headers");
            if (headersConstructor.IsUndefined() || headersConstructor.IsNull())
            {
                throw Napi::TypeError::New(env, "fetch requires Headers to be installed.");
            }

            auto normalizedHeaders = headersConstructor.As<Napi::Function>().New({headers});
            const auto callback = Napi::Function::New(env, [&request](const Napi::CallbackInfo& info) {
                if (info.Length() >= 2)
                {
                    request.SetRequestHeader(info[1].ToString().Utf8Value(), info[0].ToString().Utf8Value());
                }
            });
            normalizedHeaders.Get("forEach").As<Napi::Function>().Call(normalizedHeaders, {callback});
        }

        void InitializeFetchClasses(Napi::Env env)
        {
            auto global = env.Global();
            auto nativeObject = JsRuntime::NativeObject::GetFromJavaScript(env);
            auto exportsValue = nativeObject.Get(JS_FETCH_POLYFILL_EXPORTS_NAME);
            if (exportsValue.IsUndefined() || exportsValue.IsNull())
            {
                exportsValue = Napi::Eval(env, FetchScripts::Polyfill, "jsruntimehost://fetch-polyfill.js");
                nativeObject.Set(JS_FETCH_POLYFILL_EXPORTS_NAME, exportsValue);
            }
            const auto exports = exportsValue.As<Napi::Object>();
            const auto headers = global.Get("Headers");
            const auto response = global.Get("Response");
            if (headers.IsUndefined() || headers.IsNull() || response.IsUndefined() || response.IsNull())
            {
                global.Set("Headers", exports.Get("Headers"));
                global.Set("Response", exports.Get("Response"));
            }
            nativeObject.Set("createFetchResponse", exports.Get("createFetchResponse"));
        }

        template<typename THeaders>
        Napi::Value CreateFetchResponse(
            Napi::Env env,
            const void* body,
            size_t bodySize,
            const THeaders& headers,
            int status,
            std::string_view statusText,
            std::string_view url,
            bool redirected)
        {
            auto arrayBuffer = Napi::ArrayBuffer::New(env, bodySize);
            if (bodySize > 0)
            {
                std::memcpy(arrayBuffer.Data(), body, bodySize);
            }
            const auto bytes = Napi::Uint8Array::New(env, bodySize, arrayBuffer, 0);

            auto responseHeaders = Napi::Array::New(env, headers.size());
            uint32_t headerIndex{};
            for (const auto& header : headers)
            {
                auto pair = Napi::Array::New(env, 2);
                pair.Set(uint32_t{0}, Napi::String::New(env, header.first));
                pair.Set(uint32_t{1}, Napi::String::New(env, header.second));
                responseHeaders.Set(headerIndex++, pair);
            }

            auto init = Napi::Object::New(env);
            init.Set("headers", responseHeaders);
            init.Set("status", Napi::Number::New(env, status));
            init.Set("statusText", Napi::String::New(env, statusText.data(), statusText.size()));

            auto metadata = Napi::Object::New(env);
            metadata.Set("redirected", Napi::Boolean::New(env, redirected));
            metadata.Set("type", Napi::String::New(env, "basic"));
            metadata.Set("url", Napi::String::New(env, url.data(), url.size()));

            const auto nativeObject = JsRuntime::NativeObject::GetFromJavaScript(env);
            const auto createResponse = nativeObject.Get("createFetchResponse").As<Napi::Function>();
            return createResponse.Call(nativeObject, {env.Global().Get("Response"), bytes, init, metadata});
        }
    }

    namespace Fetch
    {
        void Initialize(Napi::Env env)
        {
            static constexpr auto JS_FETCH_NAME = "fetch";
            InitializeFetchClasses(env);

            auto fetchFunction = Napi::Function::New(env, [](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto deferred = Napi::Promise::Deferred::New(env);

                try
                {
                    if (info.Length() < 1)
                    {
                        throw std::runtime_error{"fetch requires at least 1 argument"};
                    }

                    // Resolve the request URL from a string, a Request-like object with a 'url', or anything stringifiable.
                    std::string url;
                    const Napi::Value input = info[0];
                    if (input.IsString())
                    {
                        url = input.As<Napi::String>().Utf8Value();
                    }
                    else if (input.IsObject() && input.As<Napi::Object>().Get("url").IsString())
                    {
                        url = input.As<Napi::Object>().Get("url").As<Napi::String>().Utf8Value();
                    }
                    else
                    {
                        url = input.ToString().Utf8Value();
                    }

                    UrlLib::UrlMethod method = UrlLib::UrlMethod::Get;
                    std::optional<std::string> body;
                    Napi::Value headers = env.Undefined();
                    Napi::Value signal = env.Undefined();

                    if (info.Length() > 1 && info[1].IsObject())
                    {
                        const auto init = info[1].As<Napi::Object>();

                        const auto methodValue = init.Get("method");
                        if (methodValue.IsString())
                        {
                            method = ParseMethod(methodValue.As<Napi::String>().Utf8Value());
                        }

                        const auto bodyValue = init.Get("body");
                        if (bodyValue.IsString())
                        {
                            body = bodyValue.As<Napi::String>().Utf8Value();
                        }
                        else if (!bodyValue.IsUndefined() && !bodyValue.IsNull())
                        {
                            throw std::runtime_error{"fetch: only string request bodies are supported"};
                        }

                        headers = init.Get("headers");
                        signal = init.Get("signal");
                    }

                    if (signal.IsObject())
                    {
                        const auto signalObject = signal.As<Napi::Object>();
                        if (signalObject.Get("aborted").ToBoolean().Value())
                        {
                            deferred.Reject(GetAbortReason(env, signalObject));
                            return deferred.Promise();
                        }
                    }

                    std::optional<DataUrlResponse> dataUrl;
                    try
                    {
                        dataUrl = Babylon::Polyfills::DataUrl::Parse(url);
                    }
                    catch (const std::runtime_error& error)
                    {
                        deferred.Reject(Napi::TypeError::New(env, error.what()).Value());
                        return deferred.Promise();
                    }

                    if (dataUrl)
                    {
                        if (method != UrlLib::UrlMethod::Get || body.has_value())
                        {
                            throw std::runtime_error{"fetch: data URLs only support GET requests"};
                        }
                        const std::vector<std::pair<std::string, std::string>> responseHeaders{
                            {"content-type", dataUrl->contentType}};
                        deferred.Resolve(CreateFetchResponse(
                            env,
                            dataUrl->body.data(),
                            dataUrl->body.size(),
                            responseHeaders,
                            200,
                            "OK",
                            dataUrl->url,
                            false));
                        return deferred.Promise();
                    }

                    auto request = std::make_shared<UrlLib::UrlRequest>();
                    request->Open(method, url);
                    request->ResponseType(UrlLib::UrlResponseType::Buffer);
                    ApplyRequestHeaders(*request, headers);
                    if (body)
                    {
                        request->SetRequestBody(std::move(*body));
                    }

                    // Snapshot the caller's stack now -- before SendAsync() moves work onto a worker
                    // thread -- so a transport-failure rejection can be attributed to the fetch() call
                    // site rather than to an empty scheduler tick.
                    const std::string capturedStack = CaptureCallSiteStack(env);

                    // Honor an AbortSignal passed via init.signal (WHATWG fetch). The signal is used
                    // through its JS interface (aborted / reason / add/removeEventListener) so fetch
                    // stays decoupled from the AbortController polyfill's C++ types.
                    std::shared_ptr<AbortState> abortState;
                    if (signal.IsObject())
                    {
                        const Napi::Object signalObject = signal.As<Napi::Object>();

                        // Already aborted: reject synchronously with the signal's reason, never
                        // touching the transport.
                        if (signalObject.Get("aborted").ToBoolean().Value())
                        {
                            deferred.Reject(GetAbortReason(env, signalObject));
                            return deferred.Promise();
                        }

                        abortState = std::make_shared<AbortState>();
                        abortState->signal = Napi::Persistent(signalObject);

                        Napi::Function listener = Napi::Function::New(env, [abortState, request, env](const Napi::CallbackInfo&) {
                            if (!abortState->aborted)
                            {
                                abortState->aborted = true;
                                // Node-API references cannot target primitive
                                // values, while AbortSignal reasons can be any
                                // JavaScript value.
                                Napi::Object reasonHolder = Napi::Object::New(env);
                                reasonHolder.Set("value", GetAbortReason(env, abortState->signal.Value()));
                                abortState->reasonHolder = Napi::Persistent(reasonHolder);
                                // Cancel the in-flight transport; the completion continuation then
                                // rejects with the AbortError instead of a transport TypeError.
                                request->Abort();
                            }
                        });
                        abortState->listener = Napi::Persistent(listener);
                        signalObject.Get("addEventListener").As<Napi::Function>().Call(signalObject, {Napi::String::New(env, "abort"), listener});
                    }

                    // arcana::task::then captures the scheduler by reference (see arcana task.h) and
                    // invokes it on the worker thread when the request completes -- after this fetch()
                    // call has returned. A stack-local scheduler would therefore dangle. Heap-allocate
                    // it and co-own it from the continuation so it stays alive until the request finishes.
                    auto scheduler = std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env));
                    request->SendAsync()
                        .then(*scheduler, arcana::cancellation::none(),
                            [deferred, request, env, url, capturedStack, abortState](const arcana::expected<void, std::exception_ptr>& result) {
                                // The request has settled: stop listening for aborts (breaking the
                                // listener <-> abortState ownership cycle) before deciding the outcome.
                                if (abortState)
                                {
                                    if (!abortState->signal.IsEmpty() && !abortState->listener.IsEmpty())
                                    {
                                        Napi::Object signalObject = abortState->signal.Value();
                                        signalObject.Get("removeEventListener").As<Napi::Function>().Call(signalObject, {Napi::String::New(env, "abort"), abortState->listener.Value()});
                                    }
                                    abortState->listener.Reset();
                                    abortState->signal.Reset();

                                    if (abortState->aborted)
                                    {
                                        // Per the fetch spec, an aborted request rejects with the
                                        // signal's reason (an AbortError), not a network error.
                                        deferred.Reject(abortState->reasonHolder.Value().Get("value"));
                                        return;
                                    }
                                }

                                const int status = static_cast<int>(request->StatusCode());

                                // Per the WHATWG fetch spec, only transport-level failures reject. A completed
                                // request with a non-2xx status (e.g. 404) still resolves with response.ok === false.
                                // A status of 0 indicates the transport never produced a response (network error).
                                if (result.has_error() || status == 0)
                                {
                                    // Reject with a TypeError carrying the normalized transport detail on `cause`
                                    // (built here, where the UrlRequest's ErrorString()/ErrorSymbol() are still in
                                    // scope) instead of throwing a constant string that discards them.
                                    deferred.Reject(BuildTransportError(env, *request, url, capturedStack).Value());
                                    return;
                                }

                                const auto responseBuffer = request->ResponseBuffer();
                                const auto statusText = request->StatusText();
                                const std::string responseUrl{request->ResponseUrl()};
                                const std::string_view finalUrl = responseUrl.empty() ? std::string_view{url} : std::string_view{responseUrl};
                                deferred.Resolve(CreateFetchResponse(
                                    env,
                                    responseBuffer.data(),
                                    responseBuffer.size(),
                                    request->GetAllResponseHeaders(),
                                    status,
                                    statusText,
                                    finalUrl,
                                    !responseUrl.empty() && responseUrl != url));
                            })
                        .then(*scheduler, arcana::cancellation::none(),
                            [deferred, env, scheduler](const arcana::expected<void, std::exception_ptr>& result) {
                                // A throw from the continuation above (e.g. a JS exception while building the
                                // response) lands here as an error result; surface it as a promise rejection so
                                // await fetch(...) settles. Transport failures are already rejected above, so this
                                // only handles unexpected exceptions. The scheduler is co-owned here so it
                                // outlives the in-flight request.
                                if (result.has_error())
                                {
                                    deferred.Reject(Napi::Error::New(env, result.error()).Value());
                                }
                            });
                }
                catch (...)
                {
                    deferred.Reject(Napi::Error::New(env, std::current_exception()).Value());
                }

                return deferred.Promise(); }, JS_FETCH_NAME);

            if (env.Global().Get(JS_FETCH_NAME).IsUndefined())
            {
                env.Global().Set(JS_FETCH_NAME, fetchFunction);
            }

            JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_FETCH_NAME, fetchFunction);
        }
    }
}

namespace Babylon::Polyfills::Fetch
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::Fetch::Initialize(env);
    }
}

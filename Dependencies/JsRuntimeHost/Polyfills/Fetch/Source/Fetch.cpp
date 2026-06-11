#include "Fetch.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Polyfills/Fetch.h>

#include <UrlLib/UrlLib.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        // Buffered response payload shared between the Response object and any clones it produces.
        struct ResponseData
        {
            int statusCode{};
            std::string statusText;
            std::string url;
            std::vector<std::pair<std::string, std::string>> headers;
            std::vector<std::byte> body;
        };

        bool EqualsIgnoreCase(std::string_view a, std::string_view b)
        {
            return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](unsigned char l, unsigned char r) {
                return std::tolower(l) == std::tolower(r);
            });
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

        std::optional<std::string> FindHeader(const ResponseData& data, std::string_view name)
        {
            for (const auto& header : data.headers)
            {
                if (EqualsIgnoreCase(header.first, name))
                {
                    return header.second;
                }
            }
            return std::nullopt;
        }

        void ApplyRequestHeaders(UrlLib::UrlRequest& request, const Napi::Value& headers)
        {
            if (headers.IsUndefined() || headers.IsNull())
            {
                return;
            }

            Napi::Env env = headers.Env();

            // Array of [name, value] pairs.
            if (headers.IsArray())
            {
                const auto array = headers.As<Napi::Array>();
                for (uint32_t i = 0; i < array.Length(); ++i)
                {
                    const auto pair = array.Get(i);
                    if (pair.IsArray())
                    {
                        const auto entry = pair.As<Napi::Array>();
                        request.SetRequestHeader(entry.Get(0u).ToString().Utf8Value(), entry.Get(1u).ToString().Utf8Value());
                    }
                }
                return;
            }

            if (headers.IsObject())
            {
                const auto object = headers.As<Napi::Object>();

                // Headers / Map instances expose forEach((value, key) => ...).
                const auto forEach = object.Get("forEach");
                if (forEach.IsFunction())
                {
                    const auto callback = Napi::Function::New(env, [&request](const Napi::CallbackInfo& info) {
                        if (info.Length() >= 2)
                        {
                            request.SetRequestHeader(info[1].ToString().Utf8Value(), info[0].ToString().Utf8Value());
                        }
                    });
                    forEach.As<Napi::Function>().Call(object, {callback});
                    return;
                }

                // Plain object of name/value properties.
                const auto names = object.GetPropertyNames();
                for (uint32_t i = 0; i < names.Length(); ++i)
                {
                    const auto key = names.Get(i);
                    request.SetRequestHeader(key.ToString().Utf8Value(), object.Get(key).ToString().Utf8Value());
                }
            }
        }

        Napi::Object BuildHeaders(Napi::Env env, const std::shared_ptr<ResponseData>& data)
        {
            Napi::Object headers = Napi::Object::New(env);

            headers.Set("get", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto value = FindHeader(*data, info[0].ToString().Utf8Value());
                return value ? Napi::Value{Napi::String::New(env, *value)} : Napi::Value{env.Null()};
            }, "get"));

            headers.Set("has", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                return Napi::Boolean::New(info.Env(), FindHeader(*data, info[0].ToString().Utf8Value()).has_value());
            }, "has"));

            headers.Set("forEach", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto callback = info[0].As<Napi::Function>();
                const auto thisArg = info.Length() > 1 ? info[1] : env.Undefined();
                for (const auto& header : data->headers)
                {
                    callback.Call(thisArg, {Napi::String::New(env, header.second), Napi::String::New(env, header.first)});
                }
                return env.Undefined();
            }, "forEach"));

            return headers;
        }

        Napi::Object BuildResponse(Napi::Env env, const std::shared_ptr<ResponseData>& data)
        {
            Napi::Object response = Napi::Object::New(env);

            const bool ok = data->statusCode >= 200 && data->statusCode < 300;
            response.Set("ok", Napi::Boolean::New(env, ok));
            response.Set("status", Napi::Number::New(env, data->statusCode));
            response.Set("statusText", Napi::String::New(env, data->statusText));
            response.Set("url", Napi::String::New(env, data->url));
            response.Set("redirected", Napi::Boolean::New(env, false));
            response.Set("type", Napi::String::New(env, "basic"));
            response.Set("bodyUsed", Napi::Boolean::New(env, false));
            response.Set("headers", BuildHeaders(env, data));

            response.Set("text", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto deferred = Napi::Promise::Deferred::New(env);
                std::string text{reinterpret_cast<const char*>(data->body.data()), data->body.size()};
                deferred.Resolve(Napi::String::New(env, text));
                return deferred.Promise();
            }, "text"));

            response.Set("arrayBuffer", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto deferred = Napi::Promise::Deferred::New(env);
                const auto arrayBuffer = Napi::ArrayBuffer::New(env, data->body.size());
                if (!data->body.empty())
                {
                    std::memcpy(arrayBuffer.Data(), data->body.data(), data->body.size());
                }
                deferred.Resolve(arrayBuffer);
                return deferred.Promise();
            }, "arrayBuffer"));

            response.Set("json", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto deferred = Napi::Promise::Deferred::New(env);
                std::string text{reinterpret_cast<const char*>(data->body.data()), data->body.size()};
                const auto json = env.Global().Get("JSON").As<Napi::Object>();
                const auto parse = json.Get("parse").As<Napi::Function>();
                try
                {
                    deferred.Resolve(parse.Call(json, {Napi::String::New(env, text)}));
                }
                catch (const Napi::Error& error)
                {
                    deferred.Reject(error.Value());
                }
                return deferred.Promise();
            }, "json"));

            response.Set("blob", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                Napi::Env env = info.Env();
                const auto deferred = Napi::Promise::Deferred::New(env);

                // Use IsUndefined()/IsNull() rather than IsFunction() to detect the Blob
                // polyfill: some JavaScriptCore/JSI builds classify constructor functions as
                // typeof 'object', so napi_typeof reports napi_object and IsFunction() would
                // incorrectly reject even when the Blob polyfill is installed.
                const auto blobConstructor = env.Global().Get("Blob");
                if (blobConstructor.IsUndefined() || blobConstructor.IsNull())
                {
                    deferred.Reject(Napi::Error::New(env, "fetch: Blob is not available in this environment").Value());
                    return deferred.Promise();
                }

                const auto arrayBuffer = Napi::ArrayBuffer::New(env, data->body.size());
                if (!data->body.empty())
                {
                    std::memcpy(arrayBuffer.Data(), data->body.data(), data->body.size());
                }
                const auto bytes = Napi::Uint8Array::New(env, data->body.size(), arrayBuffer, 0);

                Napi::Array parts = Napi::Array::New(env, 1);
                parts.Set(0u, bytes);

                Napi::Object options = Napi::Object::New(env);
                const auto contentType = FindHeader(*data, "content-type");
                options.Set("type", Napi::String::New(env, contentType.value_or("")));

                deferred.Resolve(blobConstructor.As<Napi::Function>().New({parts, options}));
                return deferred.Promise();
            }, "blob"));

            response.Set("clone", Napi::Function::New(env, [data](const Napi::CallbackInfo& info) -> Napi::Value {
                return BuildResponse(info.Env(), data);
            }, "clone"));

            return response;
        }
    }

    namespace Fetch
    {
        void Initialize(Napi::Env env)
        {
            static constexpr auto JS_FETCH_NAME = "fetch";

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
                    }

                    auto request = std::make_shared<UrlLib::UrlRequest>();
                    request->Open(method, url);
                    request->ResponseType(UrlLib::UrlResponseType::Buffer);
                    ApplyRequestHeaders(*request, headers);
                    if (body)
                    {
                        request->SetRequestBody(std::move(*body));
                    }

                    // arcana::task::then captures the scheduler by reference (see arcana task.h) and
                    // invokes it on the worker thread when the request completes -- after this fetch()
                    // call has returned. A stack-local scheduler would therefore dangle. Heap-allocate
                    // it and co-own it from the continuation so it stays alive until the request finishes.
                    auto scheduler = std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env));
                    request->SendAsync()
                        .then(*scheduler, arcana::cancellation::none(),
                            [deferred, request, env](const arcana::expected<void, std::exception_ptr>& result) {
                                const int status = static_cast<int>(request->StatusCode());

                                // Per the WHATWG fetch spec, only transport-level failures reject. A completed
                                // request with a non-2xx status (e.g. 404) still resolves with response.ok === false.
                                // A status of 0 indicates the transport never produced a response (network error).
                                if (result.has_error() || status == 0)
                                {
                                    throw std::runtime_error{"fetch: network request failed"};
                                }

                                auto data = std::make_shared<ResponseData>();
                                data->statusCode = status;
                                data->statusText = std::string{request->StatusText()};
                                data->url = std::string{request->ResponseUrl()};
                                for (const auto& header : request->GetAllResponseHeaders())
                                {
                                    data->headers.emplace_back(header.first, header.second);
                                }
                                const auto responseBuffer = request->ResponseBuffer();
                                data->body.assign(responseBuffer.begin(), responseBuffer.end());

                                deferred.Resolve(BuildResponse(env, data));
                            })
                        .then(*scheduler, arcana::cancellation::none(),
                            [deferred, env, scheduler](const arcana::expected<void, std::exception_ptr>& result) {
                                // A throw from the continuation above (e.g. a network failure or a JS
                                // exception while building the response) lands here as an error result;
                                // surface it as a promise rejection so await fetch(...) settles. The
                                // scheduler is co-owned here so it outlives the in-flight request.
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

                return deferred.Promise();
            }, JS_FETCH_NAME);

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

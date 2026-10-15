#include "FileReader.h"

#include <basen.hpp>

#include <cstring>
#include <iterator>
#include <memory>
#include <utility>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr auto JS_FILE_READER_CONSTRUCTOR_NAME = "FileReader";

        // base-n's encode_b64 emits unpadded base64; data: URLs use the padded
        // RFC 4648 alphabet, so append the '=' run for the final partial group.
        void EncodeBase64(const uint8_t* data, size_t size, std::string& out)
        {
            const auto* begin = reinterpret_cast<const char*>(data);
            bn::encode_b64(begin, begin + size, std::back_inserter(out));

            const size_t remainder = size % 3;
            if (remainder != 0)
            {
                out.append(3 - remainder, '=');
            }
        }

        Napi::Value MakeEvent(Napi::Env env, const Napi::Object& jsThis, const std::string& eventType)
        {
            // ProgressEvent contract: loaded/total reflect bytes processed. For
            // one-shot reads we don't track interim progress — report the final
            // byte count for both and leave lengthComputable=false.
            double length = 0.0;
            auto result = jsThis.Get("result");
            if (result.IsArrayBuffer())
            {
                length = static_cast<double>(result.As<Napi::ArrayBuffer>().ByteLength());
            }
            else if (result.IsTypedArray())
            {
                length = static_cast<double>(result.As<Napi::TypedArray>().ByteLength());
            }
            else if (result.IsString())
            {
                length = static_cast<double>(result.As<Napi::String>().Utf8Value().size());
            }

            auto event = Napi::Object::New(env);
            event.Set("type", Napi::String::New(env, eventType));
            event.Set("target", jsThis);
            event.Set("currentTarget", jsThis);
            event.Set("lengthComputable", Napi::Boolean::New(env, false));
            event.Set("loaded", Napi::Number::New(env, length));
            event.Set("total", Napi::Number::New(env, length));
            return event;
        }
    }

    void FileReader::Initialize(Napi::Env env)
    {
        auto global = env.Global();

        if (!global.Get(JS_FILE_READER_CONSTRUCTOR_NAME).IsUndefined())
        {
            return;
        }

        // Expose EMPTY/LOADING/DONE on both the constructor and the prototype
        // per the WHATWG FileAPI IDL `const` member exposure rule (see JsRH#173).
        Napi::Function func = DefineClass(
            env,
            JS_FILE_READER_CONSTRUCTOR_NAME,
            {
                StaticValue("EMPTY", Napi::Number::New(env, EMPTY)),
                StaticValue("LOADING", Napi::Number::New(env, LOADING)),
                StaticValue("DONE", Napi::Number::New(env, DONE)),
                InstanceValue("EMPTY", Napi::Number::New(env, EMPTY)),
                InstanceValue("LOADING", Napi::Number::New(env, LOADING)),
                InstanceValue("DONE", Napi::Number::New(env, DONE)),
                InstanceAccessor("readyState", &FileReader::GetReadyState, nullptr),
                InstanceAccessor("result", &FileReader::GetResult, nullptr),
                InstanceAccessor("error", &FileReader::GetError, nullptr),
                InstanceAccessor("onloadstart", &FileReader::GetOnHandler, &FileReader::SetOnHandler, napi_default, const_cast<char*>("loadstart")),
                InstanceAccessor("onprogress", &FileReader::GetOnHandler, &FileReader::SetOnHandler, napi_default, const_cast<char*>("progress")),
                InstanceAccessor("onload", &FileReader::GetOnHandler, &FileReader::SetOnHandler, napi_default, const_cast<char*>("load")),
                InstanceAccessor("onabort", &FileReader::GetOnHandler, &FileReader::SetOnHandler, napi_default, const_cast<char*>("abort")),
                InstanceAccessor("onerror", &FileReader::GetOnHandler, &FileReader::SetOnHandler, napi_default, const_cast<char*>("error")),
                InstanceAccessor("onloadend", &FileReader::GetOnHandler, &FileReader::SetOnHandler, napi_default, const_cast<char*>("loadend")),
                InstanceMethod("readAsArrayBuffer", &FileReader::ReadAsArrayBuffer),
                InstanceMethod("readAsText", &FileReader::ReadAsText),
                InstanceMethod("readAsDataURL", &FileReader::ReadAsDataURL),
                InstanceMethod("abort", &FileReader::Abort),
                InstanceMethod("addEventListener", &FileReader::AddEventListener),
                InstanceMethod("removeEventListener", &FileReader::RemoveEventListener),
                InstanceMethod("dispatchEvent", &FileReader::DispatchEvent),
            });

        global.Set(JS_FILE_READER_CONSTRUCTOR_NAME, func);
    }

    FileReader::FileReader(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<FileReader>{info}
    {
        // readyState and the on* handler slots are backed by plain C++ members.
        // result/error are boxed on a persistent holder object so the getters
        // can surface primitive (string) results without napi_create_reference
        // rejecting them on the real N-API backends.
        auto env = info.Env();
        m_state = Napi::Persistent(Napi::Object::New(env));
        m_state.Value().Set("result", env.Null());
        m_state.Value().Set("error", env.Null());
    }

    void FileReader::ReadAsArrayBuffer(const Napi::CallbackInfo& info)
    {
        StartRead(info, ReadMode::ArrayBuffer);
    }

    void FileReader::ReadAsText(const Napi::CallbackInfo& info)
    {
        StartRead(info, ReadMode::Text);
    }

    void FileReader::ReadAsDataURL(const Napi::CallbackInfo& info)
    {
        StartRead(info, ReadMode::DataUrl);
    }

    void FileReader::Abort(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        auto jsThis = info.This().As<Napi::Object>();

        if (m_readyState != LOADING)
        {
            return;
        }

        // Bump the read id so the in-flight .then continuation in
        // StartRead() early-returns instead of clobbering state and
        // dispatching a phantom "load" after the user-initiated abort.
        m_readId++;

        m_readyState = DONE;
        m_state.Value().Set("result", env.Null());
        StoreError(Napi::Error::New(env, "FileReader aborted").Value());

        Dispatch(env, jsThis, "abort");
        Dispatch(env, jsThis, "loadend");
    }

    void FileReader::AddEventListener(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 2 || !info[0].IsString() || !info[1].IsFunction())
        {
            return;
        }

        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function handler = info[1].As<Napi::Function>();

        auto& list = m_eventHandlerRefs[eventType];
        for (const auto& existing : list)
        {
            if (existing.Value() == handler)
            {
                return;
            }
        }
        list.push_back(Napi::Persistent(handler));
    }

    void FileReader::RemoveEventListener(const Napi::CallbackInfo& info)
    {
        if (info.Length() < 2 || !info[0].IsString() || !info[1].IsFunction())
        {
            return;
        }

        std::string eventType = info[0].As<Napi::String>().Utf8Value();
        Napi::Function handler = info[1].As<Napi::Function>();

        auto it = m_eventHandlerRefs.find(eventType);
        if (it == m_eventHandlerRefs.end())
        {
            return;
        }

        auto& list = it->second;
        for (auto i = list.begin(); i != list.end(); ++i)
        {
            if (i->Value() == handler)
            {
                list.erase(i);
                return;
            }
        }
    }

    Napi::Value FileReader::DispatchEvent(const Napi::CallbackInfo& info)
    {
        auto env = info.Env();
        if (info.Length() == 0 || !info[0].IsObject())
        {
            return Napi::Boolean::New(env, false);
        }

        auto eventObj = info[0].As<Napi::Object>();
        auto typeValue = eventObj.Get("type");
        if (!typeValue.IsString())
        {
            return Napi::Boolean::New(env, false);
        }

        // Route through the internal Dispatch helper so on* handlers and
        // addEventListener listeners actually fire for the event type.
        Dispatch(env, info.This().As<Napi::Object>(), typeValue.As<Napi::String>().Utf8Value());
        return Napi::Boolean::New(env, true);
    }

    void FileReader::Dispatch(Napi::Env env, const Napi::Object& jsThis, const std::string& eventType)
    {
        auto event = MakeEvent(env, jsThis, eventType);

        auto onIt = m_onHandlers.find(eventType);
        if (onIt != m_onHandlers.end() && !onIt->second.IsEmpty())
        {
            onIt->second.Value().Call(jsThis, {event});
            if (env.IsExceptionPending())
            {
                env.GetAndClearPendingException();
            }
        }

        auto it = m_eventHandlerRefs.find(eventType);
        if (it == m_eventHandlerRefs.end())
        {
            return;
        }

        // Snapshot the listener list so that mutations during dispatch
        // (e.g. a handler that calls removeEventListener) do not invalidate
        // the iterator we are walking.
        std::vector<Napi::Function> snapshot;
        snapshot.reserve(it->second.size());
        for (const auto& ref : it->second)
        {
            snapshot.push_back(ref.Value());
        }

        for (const auto& listener : snapshot)
        {
            listener.Call(jsThis, {event});
            if (env.IsExceptionPending())
            {
                env.GetAndClearPendingException();
            }
        }
    }

    void FileReader::StartRead(const Napi::CallbackInfo& info, ReadMode mode)
    {
        auto env = info.Env();
        auto jsThis = info.This().As<Napi::Object>();

        if (m_readyState == LOADING)
        {
            throw Napi::Error::New(env, "FileReader: read already in progress");
        }

        m_readyState = LOADING;
        m_state.Value().Set("result", env.Null());
        m_state.Value().Set("error", env.Null());

        // Mint a fresh id for this read. Abort()/restart bumps it to invalidate
        // a prior read's queued continuation.
        ++m_readId;
        const uint64_t myReadId = m_readId;

        Dispatch(env, jsThis, "loadstart");

        if (info.Length() == 0 || info[0].IsNull() || info[0].IsUndefined())
        {
            StoreError(Napi::Error::New(env, "FileReader: argument is not a Blob").Value());
            m_readyState = DONE;
            Dispatch(env, jsThis, "error");
            Dispatch(env, jsThis, "loadend");
            return;
        }

        Napi::Value source = info[0];
        Napi::Value promiseValue;
        std::string contentType = "application/octet-stream";

        if (source.IsObject())
        {
            auto sourceObj = source.As<Napi::Object>();

            auto typeVal = sourceObj.Get("type");
            if (typeVal.IsString())
            {
                auto t = typeVal.As<Napi::String>().Utf8Value();
                if (!t.empty())
                {
                    contentType = t;
                }
            }

            auto arrayBufferFn = sourceObj.Get("arrayBuffer");
            if (arrayBufferFn.IsFunction())
            {
                promiseValue = arrayBufferFn.As<Napi::Function>().Call(sourceObj, {});
            }
            else if (source.IsArrayBuffer())
            {
                auto promiseCtor = env.Global().Get("Promise").As<Napi::Object>();
                promiseValue = promiseCtor.Get("resolve").As<Napi::Function>().Call(promiseCtor, {source});
            }
        }

        if (!promiseValue.IsObject())
        {
            StoreError(Napi::Error::New(env, "FileReader: argument has no arrayBuffer()").Value());
            m_readyState = DONE;
            Dispatch(env, jsThis, "error");
            Dispatch(env, jsThis, "loadend");
            return;
        }

        // The .then() callbacks fire asynchronously, after StartRead() returns.
        // Per the FileAPI spec an in-flight read must keep the FileReader alive
        // even if script drops its reference, so anchor the JS wrapper for the
        // duration of the read. The anchor lives in a shared_ptr captured by
        // BOTH promise reactions (i.e. owned by the lambdas, external to the
        // wrapper) rather than in a member self-reference: when the promise
        // settles and the engine releases the reactions, the last shared_ptr
        // copy drops and the anchor is released automatically — no member
        // self-cycle and no manual Reset on each terminal path. Because the
        // wrapper is held alive until a reaction runs, `this` is always valid
        // inside the lambdas (no dead-`this` race), and `anchor->Value()`
        // yields the wrapper object. shared_ptr is copyable, so the lambdas
        // remain storable in jsi::Function's std::function-style slot even
        // though Napi::ObjectReference itself is move-only.
        auto anchor = std::make_shared<Napi::ObjectReference>(Napi::Persistent(jsThis));

        auto onResolve = Napi::Function::New(env,
            [this, anchor, myReadId, mode, contentType](const Napi::CallbackInfo& cb) {
                // Abandoned-read guard: Abort() or a restart bumped m_readId.
                if (m_readId != myReadId) return;
                Napi::Value buf = cb.Length() > 0 ? cb[0] : cb.Env().Null();
                HandleReadResult(myReadId, mode, contentType, anchor->Value(), buf);
            });

        auto onReject = Napi::Function::New(env,
            [this, anchor, myReadId](const Napi::CallbackInfo& cb) {
                if (m_readId != myReadId) return;
                Napi::Value err = cb.Length() > 0
                    ? cb[0]
                    : static_cast<Napi::Value>(Napi::Error::New(cb.Env(), "FileReader: unknown error").Value());
                HandleReadError(myReadId, anchor->Value(), err);
            });

        auto promiseObj = promiseValue.As<Napi::Object>();
        promiseObj.Get("then").As<Napi::Function>().Call(promiseObj, {onResolve, onReject});
    }

    void FileReader::HandleReadResult(uint64_t myReadId, ReadMode mode, const std::string& contentType,
                                      Napi::Object jsThis, const Napi::Value& bufValue)
    {
        // Abort()-or-restart guard: the read id was bumped, so this
        // continuation belongs to a read that has been abandoned.
        if (m_readId != myReadId)
        {
            return;
        }
        if (m_readyState != LOADING)
        {
            return;
        }

        auto env = jsThis.Env();

        if (!bufValue.IsArrayBuffer())
        {
            StoreError(Napi::Error::New(env, "FileReader: source did not return an ArrayBuffer").Value());
            m_readyState = DONE;
            Dispatch(env, jsThis, "error");
            Dispatch(env, jsThis, "loadend");
            return;
        }

        auto buffer = bufValue.As<Napi::ArrayBuffer>();
        const auto* data = static_cast<const uint8_t*>(buffer.Data());
        const size_t size = buffer.ByteLength();

        Napi::Value resultValue;
        switch (mode)
        {
            case ReadMode::ArrayBuffer:
            {
                resultValue = buffer;
                break;
            }
            case ReadMode::Text:
            {
                // Pass raw bytes to Napi::String::New, which constructs a
                // JS string from UTF-8 input. Replaces the JS polyfill's
                // chunked String.fromCharCode fallback.
                resultValue = Napi::String::New(env, reinterpret_cast<const char*>(data), size);
                break;
            }
            case ReadMode::DataUrl:
            {
                std::string b64;
                EncodeBase64(data, size, b64);
                std::string url;
                url.reserve(contentType.size() + b64.size() + 13);
                url.append("data:").append(contentType).append(";base64,").append(b64);
                resultValue = Napi::String::New(env, url);
                break;
            }
        }

        StoreResult(resultValue);
        m_readyState = DONE;
        Dispatch(env, jsThis, "load");
        Dispatch(env, jsThis, "loadend");
    }

    Napi::Value FileReader::GetReadyState(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_readyState);
    }

    Napi::Value FileReader::GetResult(const Napi::CallbackInfo&)
    {
        return m_state.Value().Get("result");
    }

    Napi::Value FileReader::GetError(const Napi::CallbackInfo&)
    {
        return m_state.Value().Get("error");
    }

    Napi::Value FileReader::GetOnHandler(const Napi::CallbackInfo& info)
    {
        const auto* eventType = static_cast<const char*>(info.Data());
        auto it = m_onHandlers.find(eventType);
        if (it != m_onHandlers.end() && !it->second.IsEmpty())
        {
            return it->second.Value();
        }
        return info.Env().Null();
    }

    void FileReader::SetOnHandler(const Napi::CallbackInfo& info, const Napi::Value& value)
    {
        const auto* eventType = static_cast<const char*>(info.Data());
        if (value.IsFunction())
        {
            m_onHandlers[eventType] = Napi::Persistent(value.As<Napi::Function>());
        }
        else
        {
            // Assigning null/undefined (or any non-function) clears the slot,
            // per the EventHandler IDL setter.
            m_onHandlers.erase(eventType);
        }
    }

    void FileReader::StoreResult(const Napi::Value& value)
    {
        m_state.Value().Set("result", value.IsEmpty() ? value.Env().Null() : value);
    }

    void FileReader::StoreError(const Napi::Value& value)
    {
        m_state.Value().Set("error", value.IsEmpty() ? value.Env().Null() : value);
    }

    void FileReader::HandleReadError(uint64_t myReadId, Napi::Object jsThis, const Napi::Value& error)
    {
        if (m_readId != myReadId)
        {
            return;
        }
        if (m_readyState != LOADING)
        {
            return;
        }

        auto env = jsThis.Env();

        if (error.IsUndefined() || error.IsNull())
        {
            StoreError(Napi::Error::New(env, "FileReader: unknown error").Value());
        }
        else
        {
            StoreError(error);
        }
        m_readyState = DONE;
        Dispatch(env, jsThis, "error");
        Dispatch(env, jsThis, "loadend");
    }
}

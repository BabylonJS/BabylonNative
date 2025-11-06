#include "Blob.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/Blob.h>

namespace Babylon::Polyfills::Internal
{
    void Blob::Initialize(Napi::Env env)
    {
        static constexpr auto JS_BLOB_CONSTRUCTOR_NAME = "Blob";
        if (env.Global().Get(JS_BLOB_CONSTRUCTOR_NAME).IsUndefined())
        {
            Napi::Function func = DefineClass(
                env,
                JS_BLOB_CONSTRUCTOR_NAME,
                {
                    InstanceAccessor("size", &Blob::GetSize, nullptr),
                    InstanceAccessor("type", &Blob::GetType, nullptr),
                    InstanceMethod("text", &Blob::Text),
                    InstanceMethod("arrayBuffer", &Blob::ArrayBuffer),
                    InstanceMethod("bytes", &Blob::Bytes),
                });

            env.Global().Set(JS_BLOB_CONSTRUCTOR_NAME, func);
        }
    }

    Blob::Blob(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<Blob>(info)
    {
        if (info.Length() > 0)
        {
            const auto blobParts = info[0].As<Napi::Array>();

            if (blobParts.Length() > 0)
            {
                const auto firstPart = blobParts.Get(uint32_t{0});
                ProcessBlobPart(firstPart);

                if (blobParts.Length() > 1)
                {
                    throw Napi::Error::New(Env(), "Using multiple BlobParts in Blob constructor is not implemented.");
                }
            }
        }

        if (info.Length() > 1)
        {
            const auto options = info[1].As<Napi::Object>();

            if (options.Has("type"))
            {
                m_type = options.Get("type").As<Napi::String>().Utf8Value();
            }
        }
    }

    Napi::Value Blob::GetSize(const Napi::CallbackInfo&)
    {
        return Napi::Value::From(Env(), m_data.size());
    }

    Napi::Value Blob::GetType(const Napi::CallbackInfo&)
    {
        return Napi::String::From(Env(), m_type);
    }

    Napi::Value Blob::Text(const Napi::CallbackInfo&)
    {
        // NOTE: This will not check for UTF-8 validity
        const auto begin = reinterpret_cast<const char*>(m_data.data());
        std::string text(begin, m_data.size());

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(Napi::String::New(Env(), text));
        return deferred.Promise();
    }

    Napi::Value Blob::ArrayBuffer(const Napi::CallbackInfo&)
    {
        const auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data.size());
        std::memcpy(arrayBuffer.Data(), m_data.data(), m_data.size());

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(arrayBuffer);
        return deferred.Promise();
    }

    Napi::Value Blob::Bytes(const Napi::CallbackInfo&)
    {
        const auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data.size());
        std::memcpy(arrayBuffer.Data(), m_data.data(), m_data.size());
        const auto uint8Array = Napi::Uint8Array::New(Env(), m_data.size(), arrayBuffer, 0);

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(uint8Array);
        return deferred.Promise();
    }

    void Blob::ProcessBlobPart(const Napi::Value& blobPart)
    {
        if (blobPart.IsArrayBuffer())
        {
            const auto buffer = blobPart.As<Napi::ArrayBuffer>();
            const auto begin = static_cast<const std::byte*>(buffer.Data());
            m_data.assign(begin, begin + buffer.ByteLength());
        }
        else if (blobPart.IsTypedArray() || blobPart.IsDataView())
        {
            const auto array = blobPart.As<Napi::TypedArray>();
            const auto buffer = array.ArrayBuffer();
            const auto begin = static_cast<const std::byte*>(buffer.Data()) + array.ByteOffset();
            m_data.assign(begin, begin + array.ByteLength());
        }
        else if (blobPart.IsString())
        {
            const auto str = blobPart.As<Napi::String>().Utf8Value();
            const auto begin = reinterpret_cast<const std::byte*>(str.data());
            m_data.assign(begin, begin + str.length());
        }
        else
        {
            // Assume it's another Blob object
            const auto obj = blobPart.As<Napi::Object>();
            const auto blobObj = Napi::ObjectWrap<Blob>::Unwrap(obj);
            m_data.assign(blobObj->m_data.begin(), blobObj->m_data.end());
        }
    }
}

namespace Babylon::Polyfills::Blob
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::Blob::Initialize(env);
    }
}

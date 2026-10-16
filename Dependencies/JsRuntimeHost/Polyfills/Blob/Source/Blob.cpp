#include "Blob.h"
#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/Blob.h>
#include <Babylon/Polyfills/BlobInternal.h>

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
        return Napi::Value::From(Env(), m_data->size());
    }

    Napi::Value Blob::GetType(const Napi::CallbackInfo&)
    {
        return Napi::String::From(Env(), m_type);
    }

    Napi::Value Blob::Text(const Napi::CallbackInfo&)
    {
        // NOTE: This will not check for UTF-8 validity
        const auto begin = reinterpret_cast<const char*>(m_data->data());
        std::string text(begin, m_data->size());

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(Napi::String::New(Env(), text));
        return deferred.Promise();
    }

    Napi::Value Blob::ArrayBuffer(const Napi::CallbackInfo&)
    {
        const auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data->size());
        if (m_data->data())
        {
            std::memcpy(arrayBuffer.Data(), m_data->data(), m_data->size());
        }

        const auto deferred = Napi::Promise::Deferred::New(Env());
        deferred.Resolve(arrayBuffer);
        return deferred.Promise();
    }

    Napi::Value Blob::Bytes(const Napi::CallbackInfo&)
    {
        const auto arrayBuffer = Napi::ArrayBuffer::New(Env(), m_data->size());
        if (m_data->data())
        {
            std::memcpy(arrayBuffer.Data(), m_data->data(), m_data->size());
        }
        const auto uint8Array = Napi::Uint8Array::New(Env(), m_data->size(), arrayBuffer, 0);

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
            m_data = std::make_shared<const std::vector<std::byte>>(begin, begin + buffer.ByteLength());
        }
        else if (blobPart.IsTypedArray() || blobPart.IsDataView())
        {
            const auto array = blobPart.As<Napi::TypedArray>();
            const auto buffer = array.ArrayBuffer();
            const auto begin = static_cast<const std::byte*>(buffer.Data()) + array.ByteOffset();
            m_data = std::make_shared<const std::vector<std::byte>>(begin, begin + array.ByteLength());
        }
        else if (blobPart.IsString())
        {
            const auto str = blobPart.As<Napi::String>().Utf8Value();
            const auto begin = reinterpret_cast<const std::byte*>(str.data());
            m_data = std::make_shared<const std::vector<std::byte>>(begin, begin + str.length());
        }
        else
        {
            // Assume it's another Blob object. Blobs are immutable, so the buffer can be shared
            // rather than copied.
            const auto obj = blobPart.As<Napi::Object>();
            const auto blobObj = Napi::ObjectWrap<Blob>::Unwrap(obj);
            m_data = blobObj->m_data;
        }
    }
}

namespace Babylon::Polyfills::Blob
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::Blob::Initialize(env);
    }

    std::optional<BlobData> BABYLON_API TryGetData(const Napi::Object& object)
    {
        const auto env = object.Env();
        const auto global = env.Global();

        // Verify `object` is a Blob (or a subclass such as File) by walking its prototype chain and
        // comparing each link against Blob.prototype, using the JS-level Object.getPrototypeOf.
        //
        // We use Object.getPrototypeOf rather than the raw napi_get_prototype C API because the
        // latter is not exposed by every adapter (e.g. JSI) and, on JSC, returns the raw
        // [[Prototype]] which differs from the JS-visible prototype of a DefineClass instance.
        // This keeps the check portable across QuickJS, V8, JavaScriptCore, Chakra and JSI, and it
        // also accepts Blob subclasses (e.g. File). We deliberately avoid napi_instanceof, whose
        // node-addon-api wrapper requires a Napi::Function.
        const auto blobConstructor = global.Get("Blob");
        if (!blobConstructor.IsFunction())
        {
            return std::nullopt;
        }

        const auto blobPrototype = blobConstructor.As<Napi::Object>().Get("prototype");
        if (!blobPrototype.IsObject())
        {
            return std::nullopt;
        }

        const auto objectConstructor = global.Get("Object");
        if (!objectConstructor.IsObject())
        {
            return std::nullopt;
        }

        const auto getPrototypeOf = objectConstructor.As<Napi::Object>().Get("getPrototypeOf");
        if (!getPrototypeOf.IsFunction())
        {
            return std::nullopt;
        }

        const auto getPrototypeOfFn = getPrototypeOf.As<Napi::Function>();

        bool isBlob = false;
        Napi::Value current = getPrototypeOfFn.Call({object});
        while (current.IsObject())
        {
            if (current.StrictEquals(blobPrototype))
            {
                isBlob = true;
                break;
            }
            current = getPrototypeOfFn.Call({current});
        }

        if (!isBlob)
        {
            return std::nullopt;
        }

        const auto* blob = Internal::Blob::Unwrap(object);
        return BlobData{blob->Data(), blob->Type()};
    }
}

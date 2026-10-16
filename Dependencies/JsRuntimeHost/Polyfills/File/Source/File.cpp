#include "File.h"
#include "FileReader.h"

#include <Babylon/Polyfills/File.h>

#include <chrono>
#include <string>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr auto JS_FILE_CONSTRUCTOR_NAME = "File";
        constexpr auto JS_BLOB_CONSTRUCTOR_NAME = "Blob";
    }

    void File::Initialize(Napi::Env env)
    {
        auto global = env.Global();

        // No-op if the runtime already provides a global File. Cheapest
        // check, and the common path on platforms with a native File.
        if (!global.Get(JS_FILE_CONSTRUCTOR_NAME).IsUndefined())
        {
            return;
        }

        // Require the native Blob polyfill: File delegates byte storage to
        // a Blob, so without it the constructor cannot produce useful
        // instances.
        auto blob = global.Get(JS_BLOB_CONSTRUCTOR_NAME);
        if (!blob.IsFunction())
        {
            throw Napi::Error::New(env,
                "File polyfill requires the Blob polyfill to be installed first.");
        }

        Napi::Function func = DefineClass(
            env,
            JS_FILE_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("size", &File::GetSize, nullptr),
                InstanceAccessor("type", &File::GetType, nullptr),
                InstanceAccessor("name", &File::GetName, nullptr),
                InstanceAccessor("lastModified", &File::GetLastModified, nullptr),
                InstanceMethod("arrayBuffer", &File::ArrayBuffer),
                InstanceMethod("text", &File::Text),
                InstanceMethod("bytes", &File::Bytes),
            });

        global.Set(JS_FILE_CONSTRUCTOR_NAME, func);

        // Wire File.prototype's [[Prototype]] to Blob.prototype so
        // `new File(...) instanceof Blob === true`. WHATWG specs File as
        // a Blob subtype; BJS core (fileTools, Offline/database,
        // abstractEngine, thinNativeEngine) branches on `instanceof Blob`
        // and needs File inputs to satisfy that check.
        auto setPrototypeOf = env.Global().Get("Object").As<Napi::Object>()
            .Get("setPrototypeOf").As<Napi::Function>();
        setPrototypeOf.Call({
            func.Get("prototype"),
            blob.As<Napi::Function>().Get("prototype"),
        });
    }

    File::File(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<File>{info}
    {
        auto env = info.Env();

        // The WHATWG File constructor takes (fileBits, fileName, [options]).
        // Both fileBits and fileName are required (USVString without
        // `optional`), so missing either is a TypeError per WebIDL bindings.
        if (info.Length() < 2)
        {
            throw Napi::TypeError::New(env,
                "Failed to construct 'File': 2 arguments required, but only " +
                std::to_string(info.Length()) + " present.");
        }

        Napi::Value parts = info[0];
        Napi::Value name = info[1];
        Napi::Value options = info.Length() > 2 ? info[2] : env.Undefined();

        // USVString conversion: undefined -> "undefined", null -> "null",
        // numbers/objects -> their .toString() representation. Napi::Value::
        // ToString() routes through napi_coerce_to_string, which matches
        // these semantics on all three engines.
        m_name = name.ToString().Utf8Value();

        // Default lastModified to the current wall clock in milliseconds,
        // matching Date.now() semantics used by the JS File constructor.
        m_lastModified = static_cast<double>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count());

        auto blobOptions = Napi::Object::New(env);

        if (options.IsObject())
        {
            auto optsObj = options.As<Napi::Object>();
            if (optsObj.Has("type"))
            {
                blobOptions.Set("type", optsObj.Get("type"));
            }
            if (optsObj.Has("lastModified"))
            {
                auto lm = optsObj.Get("lastModified");
                if (lm.IsNumber())
                {
                    m_lastModified = lm.As<Napi::Number>().DoubleValue();
                }
            }
        }

        Napi::Value partsArray;
        if (parts.IsArray())
        {
            partsArray = parts;
        }
        else
        {
            partsArray = Napi::Array::New(env, 0);
        }

        // Delegate byte-buffer construction to the native Blob polyfill so
        // we benefit from its existing BlobPart handling (ArrayBuffer,
        // typed array, string, Blob).
        auto blobCtor = env.Global().Get(JS_BLOB_CONSTRUCTOR_NAME).As<Napi::Function>();
        auto blobInstance = blobCtor.New({partsArray, blobOptions});
        m_blob = Napi::Persistent(blobInstance);
    }

    Napi::Value File::GetSize(const Napi::CallbackInfo&)
    {
        return m_blob.Value().Get("size");
    }

    Napi::Value File::GetType(const Napi::CallbackInfo&)
    {
        return m_blob.Value().Get("type");
    }

    Napi::Value File::GetName(const Napi::CallbackInfo& info)
    {
        return Napi::String::New(info.Env(), m_name);
    }

    Napi::Value File::GetLastModified(const Napi::CallbackInfo& info)
    {
        return Napi::Number::New(info.Env(), m_lastModified);
    }

    Napi::Value File::ArrayBuffer(const Napi::CallbackInfo&)
    {
        auto blob = m_blob.Value();
        return blob.Get("arrayBuffer").As<Napi::Function>().Call(blob, {});
    }

    Napi::Value File::Text(const Napi::CallbackInfo&)
    {
        auto blob = m_blob.Value();
        return blob.Get("text").As<Napi::Function>().Call(blob, {});
    }

    Napi::Value File::Bytes(const Napi::CallbackInfo&)
    {
        auto blob = m_blob.Value();
        return blob.Get("bytes").As<Napi::Function>().Call(blob, {});
    }
}

namespace Babylon::Polyfills::File
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Internal::File::Initialize(env);
        Internal::FileReader::Initialize(env);
    }
}

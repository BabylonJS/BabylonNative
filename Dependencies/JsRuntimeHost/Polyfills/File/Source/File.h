#pragma once

#include <napi/napi.h>

#include <string>

namespace Babylon::Polyfills::Internal
{
    class File final : public Napi::ObjectWrap<File>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit File(const Napi::CallbackInfo& info);

    private:
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetType(const Napi::CallbackInfo& info);
        Napi::Value GetName(const Napi::CallbackInfo& info);
        Napi::Value GetLastModified(const Napi::CallbackInfo& info);

        Napi::Value ArrayBuffer(const Napi::CallbackInfo& info);
        Napi::Value Text(const Napi::CallbackInfo& info);
        Napi::Value Bytes(const Napi::CallbackInfo& info);
        Napi::Value Slice(const Napi::CallbackInfo& info);
        Napi::Value Stream(const Napi::CallbackInfo& info);

        Napi::ObjectReference m_blob;
        std::string m_name;
        double m_lastModified{0.0};
    };
}

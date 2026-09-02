#pragma once

#include <napi/napi.h>

#include <cstddef>
#include <memory>
#include <vector>
#include <string>

namespace Babylon::Polyfills::Internal
{
    class Blob : public Napi::ObjectWrap<Blob>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit Blob(const Napi::CallbackInfo& info);

        // Synchronous accessors for internal cross-polyfill use (e.g. URL.createObjectURL).
        // A Blob is immutable once constructed, so the bytes are held in a shared_ptr and can be
        // shared with consumers rather than copied. Never null.
        const std::shared_ptr<const std::vector<std::byte>>& Data() const { return m_data; }
        const std::string& Type() const { return m_type; }

    private:
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetType(const Napi::CallbackInfo& info);
        Napi::Value Text(const Napi::CallbackInfo& info);
        Napi::Value ArrayBuffer(const Napi::CallbackInfo& info);
        Napi::Value Bytes(const Napi::CallbackInfo& info);

        void ProcessBlobPart(const Napi::Value& blobPart);

        std::shared_ptr<const std::vector<std::byte>> m_data{std::make_shared<const std::vector<std::byte>>()};
        std::string m_type;
    };
}
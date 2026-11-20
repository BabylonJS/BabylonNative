#pragma once

#include <napi/napi.h>

#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    class Blob : public Napi::ObjectWrap<Blob>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit Blob(const Napi::CallbackInfo& info);

        // Synchronous accessors for internal cross-polyfill use (e.g. URL.createObjectURL).
        // A Blob is immutable once constructed, so its bytes can be shared with consumers rather
        // than copied. Storage is segmented (slices and multi-part Blobs share their parts'
        // buffers), so Data() hands back the one underlying buffer when there is exactly one, and
        // otherwise materializes a contiguous copy once and caches it. Never null.
        const std::shared_ptr<const std::vector<std::byte>>& Data() const;
        const std::string& Type() const { return m_type; }

    private:
        Napi::Value GetSize(const Napi::CallbackInfo& info);
        Napi::Value GetType(const Napi::CallbackInfo& info);
        Napi::Value Text(const Napi::CallbackInfo& info);
        Napi::Value ArrayBuffer(const Napi::CallbackInfo& info);
        Napi::Value Bytes(const Napi::CallbackInfo& info);
        Napi::Value Slice(const Napi::CallbackInfo& info);
        Napi::Value Stream(const Napi::CallbackInfo& info);

        struct Segment;
        struct Storage;
        class StreamSource;

        bool AppendBlobPart(Storage& data, const Napi::Value& blobPart);
        Napi::ArrayBuffer CreateArrayBuffer() const;

        static std::string NormalizeType(std::string type);
        static std::string NormalizeLineEndings(std::string value);
        static size_t NormalizeSliceIndex(double value, size_t size);

        std::shared_ptr<const Storage> m_data;
        mutable std::shared_ptr<const std::vector<std::byte>> m_contiguous;
        std::string m_type;
    };
}

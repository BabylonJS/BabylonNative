#pragma once

#include <napi/env.h>
#include <Babylon/Api.h>

#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace Babylon::Polyfills::Blob
{
    // The contents of a Blob JS object created by this polyfill.
    struct BlobData
    {
        // A Blob is immutable once constructed, so this is a shared reference to the Blob's buffer
        // rather than a copy. It stays valid for as long as the caller holds it, even if the JS
        // object it came from is collected.
        std::shared_ptr<const std::vector<std::byte>> Data;

        // The Blob's MIME type, which may be empty.
        std::string Type;
    };

    // Synchronously reads the contents of `object`, which may be a Blob or a subclass such as File.
    // Returns std::nullopt if `object` is not a Blob created by this polyfill.
    std::optional<BlobData> BABYLON_API TryGetData(const Napi::Object& object);
}

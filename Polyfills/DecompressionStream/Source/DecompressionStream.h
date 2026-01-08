#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <gsl/span>
#include <vector>

#include "libdeflate.h"

namespace Babylon::Polyfills::Internal
{
    class DecompressionStream final : public Napi::ObjectWrap<DecompressionStream>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit DecompressionStream(const Napi::CallbackInfo& info);
        const std::vector<std::byte>& GetDecompressedData() const { return m_data; }
    private:
        void Enqueue(const Napi::CallbackInfo& info);
        void Close(const Napi::CallbackInfo& info);

        std::vector<std::byte> DecompressGzip(const Napi::Env env, gsl::span<uint8_t> compressedBuffer);

        std::vector<std::byte> m_data;
    };

    static constexpr auto JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME = "DecompressionStream";

    void DecompressionStream::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME,
            {
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME, func);
    }

    DecompressionStream::DecompressionStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DecompressionStream>{info}
    {
        const Napi::Env env{info.Env()};
        if (info.Length() == 1 && info[0].IsString()) {
            std::string compressionType = info[0].As<Napi::String>().Utf8Value();
            if (compressionType != "gzip") {
                throw Napi::Error::New(env, "Unsupported compression type.");
            }
        }
    }

    std::vector<std::byte> DecompressionStream::DecompressGzip(const Napi::Env env, gsl::span<uint8_t> compressedBuffer)
    {
        std::vector<std::byte> result;

        if (compressedBuffer.size() < 18) {
            throw Napi::Error::New(env, "Invalid gzip data: too small");
        }

        // Verify gzip header magic bytes (0x1f, 0x8b)
        if (compressedBuffer[0] != 0x1f || compressedBuffer[1] != 0x8b) {
            throw Napi::Error::New(env, "Invalid gzip header");
        }

        // Get uncompressed size from gzip footer (last 4 bytes, little-endian)
        size_t uncompressed_size =
            static_cast<size_t>(compressedBuffer[compressedBuffer.size() - 4]) |
            (static_cast<size_t>(compressedBuffer[compressedBuffer.size() - 3]) << 8) |
            (static_cast<size_t>(compressedBuffer[compressedBuffer.size() - 2]) << 16) |
            (static_cast<size_t>(compressedBuffer[compressedBuffer.size() - 1]) << 24);

        // Allocate output buffer
        result.resize(uncompressed_size);

        // Create decompressor
        struct libdeflate_decompressor* decompressor = libdeflate_alloc_decompressor();
        if (!decompressor) {
            throw Napi::Error::New(env, "Failed to allocate decompressor");
        }

        // Decompress gzip data
        size_t actual_size;
        enum libdeflate_result decompress_result = libdeflate_gzip_decompress(
            decompressor,
            compressedBuffer.data(),
            compressedBuffer.size(),
            result.data(),
            result.size(),
            &actual_size
        );

        // Free decompressor
        libdeflate_free_decompressor(decompressor);

        // Check result
        if (decompress_result != LIBDEFLATE_SUCCESS) {
            switch (decompress_result) {
            case LIBDEFLATE_BAD_DATA:
                    throw Napi::Error::New(env, "Gzip decompression failed: bad or corrupted data");
            case LIBDEFLATE_SHORT_OUTPUT:
                throw Napi::Error::New(env, "Gzip decompression failed: output buffer too small");
            case LIBDEFLATE_INSUFFICIENT_SPACE:
                throw Napi::Error::New(env, "Gzip decompression failed: insufficient space");
            default:
                throw Napi::Error::New(env, "Gzip decompression failed: unknown error");
            }
        }

        // Resize to actual decompressed size (in case it differs from footer)
        result.resize(actual_size);

        return result;
    }


}

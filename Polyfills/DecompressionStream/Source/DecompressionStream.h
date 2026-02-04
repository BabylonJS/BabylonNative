#pragma once

#include <vector>
#include <libdeflate.h>

namespace Babylon::Polyfills::Internal
{
    class DecompressionStream final : public Napi::ObjectWrap<DecompressionStream>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit DecompressionStream(const Napi::CallbackInfo& info);
        ~DecompressionStream();

        void AppendData(const uint8_t* data, size_t length);
        void Finish();
        std::vector<std::byte> GetDecompressedData() const;

        Napi::Value GetReadable(const Napi::CallbackInfo& info);
        Napi::Value GetWritable(const Napi::CallbackInfo& info);

    private:
        std::string m_format;
        std::vector<uint8_t> m_compressedData;
        std::vector<std::byte> m_decompressedData;
        struct libdeflate_decompressor* m_decompressor{nullptr};
    };

    static constexpr auto JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME = "DecompressionStream";

    void DecompressionStream::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{env};

        Napi::Function func = DefineClass(
            env,
            JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME,
            {
                InstanceAccessor("readable", &DecompressionStream::GetReadable, nullptr),
                InstanceAccessor("writable", &DecompressionStream::GetWritable, nullptr),
            });

        env.Global().Set(JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME, func);
    }

    DecompressionStream::DecompressionStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DecompressionStream>{info}
        , m_format{"gzip"}
    {
        if (info.Length() > 0 && info[0].IsString())
        {
            m_format = info[0].As<Napi::String>().Utf8Value();
        }

        m_decompressor = libdeflate_alloc_decompressor();
    }

    DecompressionStream::~DecompressionStream()
    {
        if (m_decompressor)
        {
            libdeflate_free_decompressor(m_decompressor);
            m_decompressor = nullptr;
        }
    }

    void DecompressionStream::AppendData(const uint8_t* data, size_t length)
    {
        // libdeflate requires all data upfront, so accumulate chunks
        m_compressedData.insert(m_compressedData.end(), data, data + length);
    }

    void DecompressionStream::Finish()
    {
        if (!m_decompressor || m_compressedData.empty())
            return;

        // Estimate output size (start with 4x input, grow if needed)
        size_t outputCapacity = m_compressedData.size() * 4;
        std::vector<uint8_t> outputBuffer(outputCapacity);
        size_t actualOutBytes = 0;
        libdeflate_result result;

        do
        {
            if (m_format == "gzip")
            {
                result = libdeflate_gzip_decompress(
                    m_decompressor,
                    m_compressedData.data(), m_compressedData.size(),
                    outputBuffer.data(), outputBuffer.size(),
                    &actualOutBytes);
            }
            else if (m_format == "deflate-raw")
            {
                result = libdeflate_deflate_decompress(
                    m_decompressor,
                    m_compressedData.data(), m_compressedData.size(),
                    outputBuffer.data(), outputBuffer.size(),
                    &actualOutBytes);
            }
            else // "deflate" (zlib wrapper)
            {
                result = libdeflate_zlib_decompress(
                    m_decompressor,
                    m_compressedData.data(), m_compressedData.size(),
                    outputBuffer.data(), outputBuffer.size(),
                    &actualOutBytes);
            }

            if (result == LIBDEFLATE_INSUFFICIENT_SPACE)
            {
                outputCapacity *= 2;
                outputBuffer.resize(outputCapacity);
            }
        }
        while (result == LIBDEFLATE_INSUFFICIENT_SPACE);

        if (result == LIBDEFLATE_SUCCESS)
        {
            m_decompressedData.reserve(actualOutBytes);
            for (size_t i = 0; i < actualOutBytes; ++i)
            {
                m_decompressedData.push_back(static_cast<std::byte>(outputBuffer[i]));
            }
        }

        // Clear compressed data after processing
        m_compressedData.clear();
    }

    std::vector<std::byte> DecompressionStream::GetDecompressedData() const
    {
        return m_decompressedData;
    }

    Napi::Value DecompressionStream::GetReadable(const Napi::CallbackInfo& info)
    {
        return info.This();
    }

    Napi::Value DecompressionStream::GetWritable(const Napi::CallbackInfo& info)
    {
        return info.This();
    }
}
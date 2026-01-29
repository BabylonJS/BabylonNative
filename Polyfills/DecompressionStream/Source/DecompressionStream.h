#pragma once

#include <Babylon/JsRuntimeScheduler.h>
#include <vector>
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4505) // error C4505: '' : unreferenced local function has been removed
#endif
#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
#endif
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-function"
#endif

#include "miniz.h"

#ifdef _MSC_VER
#pragma warning(pop)
#endif
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif
#ifdef __clang__
#pragma clang diagnostic pop
#endif

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
        std::vector<std::byte> m_decompressedData;
        mz_stream m_zstream{};
        bool m_initialized{false};
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

        m_zstream.zalloc = Z_NULL;
        m_zstream.zfree = Z_NULL;
        m_zstream.opaque = Z_NULL;

        int windowBits = 15;
        if (m_format == "gzip")
        {
            windowBits += 16; // gzip format
        }
        else if (m_format == "deflate-raw")
        {
            windowBits = -15; // raw deflate
        }

        if (inflateInit2(&m_zstream, windowBits) == Z_OK)
        {
            m_initialized = true;
        }
    }

    DecompressionStream::~DecompressionStream()
    {
        if (m_initialized)
        {
            inflateEnd(&m_zstream);
        }
    }

    void DecompressionStream::AppendData(const uint8_t* data, size_t length)
    {
        if (!m_initialized)
            return;

        m_zstream.next_in = const_cast<Bytef*>(data);
        m_zstream.avail_in = static_cast<uInt>(length);

        std::vector<uint8_t> buffer(32768);
        do
        {
            m_zstream.next_out = buffer.data();
            m_zstream.avail_out = static_cast<uInt>(buffer.size());

            int ret = inflate(&m_zstream, Z_NO_FLUSH);
            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            {
                break;
            }

            size_t have = buffer.size() - m_zstream.avail_out;
            for (size_t i = 0; i < have; ++i)
            {
                m_decompressedData.push_back(static_cast<std::byte>(buffer[i]));
            }
        }
        while (m_zstream.avail_out == 0);
    }

    void DecompressionStream::Finish()
    {
        if (m_initialized)
        {
            inflateEnd(&m_zstream);
            m_initialized = false;
        }
    }

    std::vector<std::byte> DecompressionStream::GetDecompressedData() const
    {
        return m_decompressedData;
    }

    Napi::Value DecompressionStream::GetReadable(const Napi::CallbackInfo& info)
    {
        // Return this object as the readable stream for piping
        return info.This();
    }

    Napi::Value DecompressionStream::GetWritable(const Napi::CallbackInfo& info)
    {
        // Return this object as the writable stream
        return info.This();
    }
}
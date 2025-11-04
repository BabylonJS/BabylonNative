#include <Babylon/JsRuntimeScheduler.h>
#include <gsl/span>
#include <vector>

BX_PRAGMA_DIAGNOSTIC_PUSH();
BX_PRAGMA_DIAGNOSTIC_IGNORED_CLANG_GCC("-Wunused-function");
BX_PRAGMA_DIAGNOSTIC_IGNORED_MSVC(4505) // error C4505: '' : unreferenced local function has been removed

#include "miniz.h"

BX_PRAGMA_DIAGNOSTIC_POP();


namespace Babylon::Plugins::Internal
{
    class DecompressionStream final : public Napi::ObjectWrap<DecompressionStream>
    {
    public:
        static void Initialize(Napi::Env env);

        explicit DecompressionStream(const Napi::CallbackInfo& info);
    private:
        void Enqueue(const Napi::CallbackInfo& info);
        void Close(const Napi::CallbackInfo& info);

        std::vector<uint8_t> DecompressGzip(gsl::span<const std::byte> compressedBuffer);
    };

    static constexpr auto JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME = "DecompressionStream";

    void DecompressionStream::Initialize(Napi::Env env)
    {
        Napi::HandleScope scope{ env };

        Napi::Function func = DefineClass(
            env,
            JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME,
            {
                InstanceMethod("enqueue", &DecompressionStream::Enqueue),
                InstanceMethod("close", &DecompressionStream::Close),
            });

        JsRuntime::NativeObject::GetFromJavaScript(env).Set(JS_DECOMPRESSIONSTREAM_CONSTRUCTOR_NAME, func);
    }

    DecompressionStream::DecompressionStream(const Napi::CallbackInfo& info)
        : Napi::ObjectWrap<DecompressionStream>{ info }
    {
        const Napi::Env env{ info.Env() };
        if (info.Length() < 1 || !info[0].IsString()) {
            throw Napi::Error::New(env, "Expected String argument for DecompressionStream constructor.");
        }
        std::string compressionType = info[0].As<Napi::String>().Utf8Value();
        if (compressionType != "gzip") {
            throw Napi::Error::New(env, "Unexpected compression type.");
        }
    }

    std::vector<uint8_t> DecompressionStream::DecompressGzip(gsl::span<const std::byte> compressedBuffer)
    {
        mz_stream stream{};
        stream.next_in = reinterpret_cast<const unsigned char*>(compressedBuffer.data());
        stream.avail_in = static_cast<unsigned long>(compressedBuffer.size());

        // 16 + MAX_WBITS tells zlib to expect gzip headers
        int ret = mz_inflateInit2(&stream, 16 + MZ_DEFAULT_WINDOW_BITS);
        if (ret != MZ_OK)
        {
            throw std::runtime_error("mz_inflateInit2() failed");
        }

        std::vector<uint8_t> decompressed;
        decompressed.resize(compressedBuffer.size() * 2); // start with a guess

        do
        {
            if (stream.total_out >= decompressed.size())
            {
                decompressed.resize(decompressed.size() * 2);
            }

            stream.next_out = decompressed.data() + stream.total_out;
            stream.avail_out = static_cast<unsigned long>(decompressed.size() - stream.total_out);

            ret = mz_inflate(&stream, MZ_NO_FLUSH);

            if (ret != MZ_OK && ret != MZ_STREAM_END)
            {
                mz_inflateEnd(&stream);
                throw std::runtime_error("mz_inflate() failed");
            }
        } while (ret != MZ_STREAM_END);

        decompressed.resize(stream.total_out);
        mz_inflateEnd(&stream);

        return decompressed;
    }

    void DecompressionStream::Enqueue(const Napi::CallbackInfo& info)
    {
        const Napi::Env env{ info.Env() };
        auto value = info[0];
        Napi::TypedArray typed = value.As<Napi::TypedArray>();

        if (typed.TypedArrayType() == napi_uint8_array)
        {
            Napi::Uint8Array array = typed.As<Napi::Uint8Array>();
            
            gsl::span<const std::byte> buffer = { reinterpret_cast<std::byte*>(array.Data()), array.ByteLength() };
            if (buffer.empty())
            {
                throw Napi::Error::New(env, "GZip data buffer is empty.");
            }

            std::vector<uint8_t> decompressedStream = DecompressGzip(buffer);
        }
    }


    void DecompressionStream::Close(const Napi::CallbackInfo& /*info*/)
    {
    }
}

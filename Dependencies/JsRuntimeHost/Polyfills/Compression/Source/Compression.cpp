#include "CompressionScripts.h"

#include <Babylon/JsRuntime.h>
#include <Babylon/Polyfills/Compression.h>
#include <Babylon/Polyfills/Streams.h>

#include <napi/napi.h>
#include <zlib.h>

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace Babylon::Polyfills::Internal
{
    namespace
    {
        constexpr size_t OUTPUT_BUFFER_SIZE = 64 * 1024;

        int WindowBits(const std::string& format)
        {
            if (format == "deflate")
            {
                return 15;
            }
            if (format == "deflate-raw")
            {
                return -15;
            }
            if (format == "gzip")
            {
                return 15 + 16;
            }
            return 0;
        }

        class CompressionCodec final : public Napi::ObjectWrap<CompressionCodec>
        {
        public:
            static Napi::Function CreateConstructor(Napi::Env env)
            {
                return DefineClass(
                    env,
                    "NativeCompressionCodec",
                    {
                        InstanceMethod("transform", &CompressionCodec::Transform),
                        InstanceMethod("finish", &CompressionCodec::Finish),
                        InstanceMethod("close", &CompressionCodec::Close),
                    });
            }

            explicit CompressionCodec(const Napi::CallbackInfo& info)
                : Napi::ObjectWrap<CompressionCodec>{info}
            {
                auto env = info.Env();
                if (info.Length() < 2 || !info[0].IsString())
                {
                    throw Napi::TypeError::New(env, "Invalid compression codec arguments");
                }

                const auto format = info[0].As<Napi::String>().Utf8Value();
                const auto windowBits = WindowBits(format);
                if (windowBits == 0)
                {
                    throw Napi::TypeError::New(env, "Unsupported compression format");
                }

                m_compressing = info[1].ToBoolean().Value();
                const auto result = m_compressing
                                        ? deflateInit2(&m_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, windowBits, 8, Z_DEFAULT_STRATEGY)
                                        : inflateInit2(&m_stream, windowBits);
                if (result != Z_OK)
                {
                    throw Napi::Error::New(env, "Unable to initialize the compression codec");
                }
                m_initialized = true;
            }

            ~CompressionCodec() override
            {
                EndStream();
            }

        private:
            Napi::Value Transform(const Napi::CallbackInfo& info)
            {
                auto env = info.Env();
                if (m_closed || m_finished)
                {
                    throw Napi::TypeError::New(env, "The compression stream is no longer writable");
                }
                if (info.Length() < 2 || !info[0].IsTypedArray() || !info[1].IsFunction())
                {
                    throw Napi::TypeError::New(env, "Invalid compression transform arguments");
                }

                const auto input = info[0].As<Napi::TypedArray>();
                if (input.TypedArrayType() != napi_uint8_array)
                {
                    throw Napi::TypeError::New(env, "Compression input must be a Uint8Array");
                }

                const auto bytes = info[0].As<Napi::Uint8Array>();
                if (bytes.ByteLength() > 0)
                {
                    if (!Process(bytes.Data(), bytes.ByteLength(), false, info[1].As<Napi::Function>()))
                    {
                        return {};
                    }
                }
                return env.Undefined();
            }

            Napi::Value Finish(const Napi::CallbackInfo& info)
            {
                auto env = info.Env();
                if (!m_closed && !m_finished)
                {
                    if (info.Length() < 1 || !info[0].IsFunction())
                    {
                        throw Napi::TypeError::New(env, "Invalid compression finish arguments");
                    }
                    if (!Process(nullptr, 0, true, info[0].As<Napi::Function>()))
                    {
                        return {};
                    }
                }
                return env.Undefined();
            }

            Napi::Value Close(const Napi::CallbackInfo& info)
            {
                m_closed = true;
                EndStream();
                ReleasePendingStorage();
                return info.Env().Undefined();
            }

            void EnsureOutputBuffer()
            {
                if (!m_outputBuffer)
                {
                    m_outputBuffer = std::make_unique<uint8_t[]>(OUTPUT_BUFFER_SIZE);
                }
            }

            void CaptureOutput(size_t byteLength)
            {
                if (byteLength == 0)
                {
                    return;
                }

                // Stage bytes rather than JavaScript arrays: a Uint8Array created here would be
                // referenced only from this vector until EnqueuePending, and the JavaScriptCore
                // backend roots nothing that is not on the C stack or explicitly protected, so a
                // collection triggered by allocating the next chunk could reclaim an earlier one
                // (observed as a corrupted first chunk under JSC_collectContinuously=1).
                m_pendingOutput.emplace_back(m_outputBuffer.get(), m_outputBuffer.get() + byteLength);
            }

            void EnqueuePending(const Napi::Function& enqueue)
            {
                const auto env = enqueue.Env();
                for (const auto& bytes : m_pendingOutput)
                {
                    auto output = Napi::Uint8Array::New(env, bytes.size());
                    std::memcpy(output.Data(), bytes.data(), bytes.size());
                    enqueue.Call({output});
                }
                m_pendingOutput.clear();
            }

            bool Process(const uint8_t* input, size_t inputLength, bool finishing, const Napi::Function& enqueue)
            {
                auto env = enqueue.Env();
                if (!m_initialized)
                {
                    throw Napi::TypeError::New(env, "The compression stream is no longer writable");
                }

                EnsureOutputBuffer();
                m_pendingOutput.clear();

                const uint8_t* inputCursor = input;
                size_t inputRemaining = inputLength;
                bool reachedEnd{};
                bool closeAfterEnqueue{};
                std::string failure;

                try
                {
                    while (true)
                    {
                        if (m_stream.avail_in == 0 && inputRemaining > 0)
                        {
                            const auto nextLength = std::min(
                                inputRemaining,
                                static_cast<size_t>(std::numeric_limits<uInt>::max()));
                            m_stream.next_in = const_cast<Bytef*>(reinterpret_cast<const Bytef*>(inputCursor));
                            m_stream.avail_in = static_cast<uInt>(nextLength);
                            inputCursor += nextLength;
                            inputRemaining -= nextLength;
                        }

                        m_stream.next_out = reinterpret_cast<Bytef*>(m_outputBuffer.get());
                        m_stream.avail_out = static_cast<uInt>(OUTPUT_BUFFER_SIZE);

                        const auto inputBefore = m_stream.avail_in;
                        const auto result = m_compressing
                                                ? deflate(&m_stream, finishing ? Z_FINISH : Z_NO_FLUSH)
                                                : inflate(&m_stream, finishing ? Z_FINISH : Z_NO_FLUSH);
                        const auto produced = OUTPUT_BUFFER_SIZE - m_stream.avail_out;
                        CaptureOutput(produced);

                        if (result == Z_STREAM_END)
                        {
                            reachedEnd = true;
                            closeAfterEnqueue = true;
                            if (!m_compressing && (m_stream.avail_in > 0 || inputRemaining > 0))
                            {
                                failure = "Unexpected input after the end of the compressed stream";
                            }
                            break;
                        }

                        if (result == Z_DATA_ERROR || result == Z_NEED_DICT)
                        {
                            failure = m_stream.msg != nullptr
                                          ? std::string{"The compressed data is invalid: "} + m_stream.msg
                                          : "The compressed data is invalid";
                            closeAfterEnqueue = true;
                            break;
                        }
                        if (result == Z_MEM_ERROR)
                        {
                            failure = "The compression codec ran out of memory";
                            closeAfterEnqueue = true;
                            break;
                        }
                        if (result != Z_OK && result != Z_BUF_ERROR)
                        {
                            failure = "The compression codec entered an invalid state";
                            closeAfterEnqueue = true;
                            break;
                        }

                        const bool madeProgress = produced > 0 || m_stream.avail_in < inputBefore;
                        const bool hasInput = m_stream.avail_in > 0 || inputRemaining > 0;
                        if (!finishing && !hasInput && m_stream.avail_out > 0)
                        {
                            break;
                        }
                        if (!madeProgress && result == Z_BUF_ERROR)
                        {
                            if (finishing && !m_compressing)
                            {
                                failure = "The compressed input ended before the end of the stream";
                            }
                            else
                            {
                                failure = "The compression codec could not make progress";
                            }
                            closeAfterEnqueue = true;
                            break;
                        }
                        if (!madeProgress && finishing)
                        {
                            failure = m_compressing
                                          ? "The compression codec could not finish"
                                          : "The compressed input ended before the end of the stream";
                            closeAfterEnqueue = true;
                            break;
                        }
                    }
                }
                catch (...)
                {
                    ResetZlibPointers();
                    m_pendingOutput.clear();
                    m_closed = true;
                    EndStream();
                    ReleasePendingStorage();
                    throw;
                }

                ResetZlibPointers();
                if (closeAfterEnqueue)
                {
                    EndStream();
                }

                try
                {
                    // Complete zlib's access to the borrowed input before this
                    // callback can run JavaScript and mutate or detach it.
                    EnqueuePending(enqueue);
                }
                catch (...)
                {
                    m_closed = true;
                    EndStream();
                    ReleasePendingStorage();
                    throw;
                }

                if (!failure.empty())
                {
                    m_closed = true;
                    ReleasePendingStorage();
                    // This error is reported after EnqueuePending has called
                    // back into JavaScript. ThrowAsJavaScriptException only sets
                    // the pending exception (it never throws C++), which avoids a
                    // second C++ exception conversion in Node-API backends with
                    // reentrant callback contexts -- and unlike the raw C API it
                    // also exists on the JSI backend, which implements no napi_*.
                    Napi::TypeError::New(Napi::Env{env}, failure).ThrowAsJavaScriptException();
                    return false;
                }

                if (reachedEnd || finishing)
                {
                    m_finished = true;
                    EndStream();
                    ReleasePendingStorage();
                }
                return true;
            }

            void ResetZlibPointers() noexcept
            {
                m_stream.next_in = Z_NULL;
                m_stream.avail_in = 0;
                m_stream.next_out = Z_NULL;
                m_stream.avail_out = 0;
            }

            void EndStream() noexcept
            {
                if (m_initialized)
                {
                    if (m_compressing)
                    {
                        deflateEnd(&m_stream);
                    }
                    else
                    {
                        inflateEnd(&m_stream);
                    }
                    m_initialized = false;
                }
                m_outputBuffer.reset();
            }

            void ReleasePendingStorage()
            {
                m_pendingOutput.clear();
                std::vector<std::vector<uint8_t>>{}.swap(m_pendingOutput);
            }

            z_stream m_stream{};
            std::unique_ptr<uint8_t[]> m_outputBuffer;
            std::vector<std::vector<uint8_t>> m_pendingOutput;
            bool m_compressing{};
            bool m_initialized{};
            bool m_finished{};
            bool m_closed{};
        };
    }
}

namespace Babylon::Polyfills::Compression
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        Streams::Initialize(env);

        Napi::HandleScope scope{env};
        auto global = env.Global();
        const auto compressionStream = global.Get("CompressionStream");
        const auto decompressionStream = global.Get("DecompressionStream");
        if (!compressionStream.IsUndefined() && !decompressionStream.IsUndefined())
        {
            return;
        }

        if (!global.Get("TransformStream").IsFunction())
        {
            throw Napi::Error::New(env, "Compression streams require TransformStream");
        }

        const auto nativeConstructor = Internal::CompressionCodec::CreateConstructor(env);
        const auto factory = Napi::Eval(
            env,
            Internal::CompressionScripts::Polyfill,
            "jsruntimehost://compression-polyfill.js")
                                 .As<Napi::Function>();
        const auto exports = factory.Call({nativeConstructor}).As<Napi::Object>();

        if (compressionStream.IsUndefined())
        {
            global.Set("CompressionStream", exports.Get("CompressionStream"));
        }
        if (decompressionStream.IsUndefined())
        {
            global.Set("DecompressionStream", exports.Get("DecompressionStream"));
        }
    }
}

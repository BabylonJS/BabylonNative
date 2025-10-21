#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/napi.h>

#include <gsl/gsl>

#include <bimg/encode.h>
#include <bx/readerwriter.h>

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

// TODO - Cleanup pass
namespace Babylon::Plugins
{
    namespace
    {
        std::vector<uint8_t> EncodePNG(const gsl::span<const uint8_t> pixelData, uint32_t width, uint32_t height, bool invertY)
        {
            bx::MemoryBlock memoryBlock{&Graphics::DeviceContext::GetDefaultAllocator()};
            bx::MemoryWriter writer{&memoryBlock};
            bx::Error err;

            bimg::imageWritePng(&writer, width, height, width * 4, pixelData.data(), bimg::TextureFormat::RGBA8, !invertY, &err);

            const auto byteLength{memoryBlock.getSize()};

            if (!err.isOk())
            {
                throw std::runtime_error("Failed to encode PNG image: " + std::string(err.getMessage().getCPtr()));
            }

            if (byteLength == 0)
            {
                throw std::runtime_error("Failed to encode PNG image: output is empty");
            }

            // TODO - Return MemoryBlock directly?
            std::vector<uint8_t> result(byteLength);
            std::memcpy(result.data(), memoryBlock.more(0), byteLength);

            return result;
        }

        Napi::Promise EncodeImageAsync(const Napi::CallbackInfo& info)
        {
            const auto buffer{info[0].As<Napi::Uint8Array>()};
            const auto width{info[1].As<Napi::Number>().Uint32Value()};
            const auto height{info[2].As<Napi::Number>().Uint32Value()};
            const auto mimeType{info[3].As<Napi::String>().Utf8Value()};
            const auto invertY{info[4].As<Napi::Boolean>().Value()};
            
            const auto env{info.Env()};
            const auto deferred{Napi::Promise::Deferred::New(env)};
            const auto promise{deferred.Promise()};

            if (buffer.ByteLength() != width * height * 4)
            {
                deferred.Reject(Napi::RangeError::New(env, "Buffer byte length does not match RGBA8 image of provided dimensions.").Value());
                return promise;
            }

            if (mimeType != "image/png")
            {
                deferred.Reject(Napi::Error::New(env, "Unsupported mime type: " + mimeType + ". Only image/png is currently supported.").Value());
                return promise;
            }

            auto runtimeScheduler{std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env))};
            const auto bufferSpan{gsl::make_span(buffer.Data(), buffer.ByteLength())};

            arcana::make_task(arcana::threadpool_scheduler, arcana::cancellation_source::none(),
                [bufferSpan, width, height, invertY]() -> std::vector<uint8_t> {
                    // TODO - There are problems accessing it on a background thread.
                    // There are two types of scenarios to consider:
                    // 1. Garbage collection: The JS buffer falls out of scope during normal execution. It gets GC'd.
                    // 2. Runtime shutdown: The entire JS environment, including the buffer, is torn down.
                    // The key difference: `Napi::Persistent` protects against scenario #1 but not #2. If async operations can outlive the runtime lifecycle, I guess some options are:
                    // - Copy the data to avoid any N-API object lifetime dependencies, or
                    // - Give this guy some state and a dtor that cancels pending operations (assuming dtor would run before runtime shutdown and that cancellation would even help), or
                    // - Secret third option?
                    // I haven't given the Napi::Promise any thought yet either, if that needs similar protection.
                    return EncodePNG(bufferSpan, width, height, invertY);
                })
                .then(*runtimeScheduler, arcana::cancellation_source::none(),
                    // NOTE - Keep references to runtimeScheduler and buffer alive until this lambda is invoked
                    [runtimeScheduler, dataRef{Napi::Persistent(buffer)}, deferred, env](const arcana::expected<std::vector<uint8_t>, std::exception_ptr>& result) {
                        if (result.has_error())
                        {
                            deferred.Reject(Napi::Error::New(env, result.error()).Value());
                            return;
                        }

                        const auto& encodedData{result.value()};
                        const auto arrayBuffer{Napi::ArrayBuffer::New(env, encodedData.size())};
                        std::memcpy(arrayBuffer.Data(), encodedData.data(), encodedData.size());

                        deferred.Resolve(arrayBuffer);
                    });

            return promise;
        }
    }
}

namespace Babylon::Plugins::NativeEncoding
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        const auto native{JsRuntime::NativeObject::GetFromJavaScript(env)};
        native.Set("EncodeImageAsync", Napi::Function::New(env, EncodeImageAsync, "EncodeImageAsync"));
    }
}
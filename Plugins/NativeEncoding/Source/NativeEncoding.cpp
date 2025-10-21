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
        // Manager to own cancellation source with proper lifetime
        // TODO - Move to shared location if needed elsewhere, like for global async ops
        class EncodingManager final
        {
        public:
            const std::shared_ptr<arcana::cancellation_source> GetCancellationSource() const
            { 
                return m_cancellationSource; 
            }

            static EncodingManager& GetFromJavaScript(Napi::Env env)
            {
                auto nativeObject = JsRuntime::NativeObject::GetFromJavaScript(env);
                auto managerValue = nativeObject.Get("_encodingManager");

                if (managerValue.IsUndefined())
                {
                    auto manager = new EncodingManager();
                    auto external = Napi::External<EncodingManager>::New(
                        env,
                        manager,
                        [](Napi::Env, EncodingManager* mgr) {
                            mgr->m_cancellationSource->cancel(); // Cancel all pending operations on teardown
                            delete mgr;
                        });
                    nativeObject.Set("_encodingManager", external);
                    return *manager;
                }

                return *managerValue.As<Napi::External<EncodingManager>>().Data();
            }

        private:
            EncodingManager()
                : m_cancellationSource{std::make_shared<arcana::cancellation_source>()}
            {
            }

            const std::shared_ptr<arcana::cancellation_source> m_cancellationSource;
        };

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

            auto& manager{EncodingManager::GetFromJavaScript(env)};
            auto& cancellationSource = manager.GetCancellationSource();
            auto runtimeScheduler{std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env))};
            std::vector<uint8_t> bufferCopy(buffer.Data(), buffer.Data() + buffer.ByteLength()); // Avoid JS lifetime issues in async work

            arcana::make_task(arcana::threadpool_scheduler, *cancellationSource,
                [pixelData{std::move(bufferCopy)}, width, height, invertY]() -> std::vector<uint8_t> {
                    return EncodePNG(gsl::make_span(pixelData), width, height, invertY);
                })
                .then(*runtimeScheduler, *cancellationSource,
                    // NOTE - Keep references to runtimeScheduler and cancellationSource alive until this lambda is invoked
                    [runtimeScheduler, cancellationSource, deferred, env](const arcana::expected<std::vector<uint8_t>, std::exception_ptr>& result) {
                        if (cancellationSource->cancelled()) 
                        {
                            // JS runtime is being torn down, do not attempt to resolve/reject the promise
                            return;
                        }
                        
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
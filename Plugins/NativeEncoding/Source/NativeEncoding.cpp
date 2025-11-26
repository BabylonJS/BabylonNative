#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/JsRuntimeScheduler.h>
#include <Babylon/Graphics/DeviceContext.h>

#include <napi/napi.h>

#include <bimg/encode.h>
#include <bx/readerwriter.h>

#include <arcana/threading/task.h>
#include <arcana/threading/task_schedulers.h>

namespace Babylon::Plugins
{
    namespace
    {
        std::shared_ptr<std::vector<std::byte>> EncodePNG(const std::vector<std::byte>& pixelData, uint32_t width, uint32_t height, bool invertY)
        {
            auto memoryBlock{bx::MemoryBlock(&Graphics::DeviceContext::GetDefaultAllocator())};
            auto writer{bx::MemoryWriter(&memoryBlock)};
            auto err{bx::Error()};

            bimg::imageWritePng(&writer, width, height, width * 4, pixelData.data(), bimg::TextureFormat::RGBA8, !invertY, &err);

            auto byteLength{memoryBlock.getSize()};

            if (!err.isOk())
            {
                throw std::runtime_error("Failed to encode PNG image: " + std::string(err.getMessage().getCPtr()));
            }

            if (byteLength == 0)
            {
                throw std::runtime_error("Failed to encode PNG image: output is empty");
            }

            auto result{std::make_shared<std::vector<std::byte>>(byteLength)};
            std::memcpy(result->data(), memoryBlock.more(0), byteLength);

            return result;
        }

        Napi::Value EncodeImageAsync(const Napi::CallbackInfo& info)
        {
            auto buffer{info[0].As<Napi::TypedArray>()}; // ArrayBufferView
            auto width{info[1].As<Napi::Number>().Uint32Value()};
            auto height{info[2].As<Napi::Number>().Uint32Value()};
            auto mimeType{info.Length() > 3 && !info[3].IsUndefined() ? info[3].As<Napi::String>().Utf8Value() : "image/png"};
            auto invertY{info.Length() > 4 && !info[4].IsUndefined() ? info[4].As<Napi::Boolean>().Value() : false};

            auto env{info.Env()};
            auto deferred{Napi::Promise::Deferred::New(env)};

            if (buffer.ByteLength() != width * height * 4)
            {
                deferred.Reject(Napi::Error::New(env, "Buffer byte length does not match RGBA8 format (4 bytes per pixel) of provided dimensions.").Value());
                return deferred.Promise();
            }

            auto runtimeScheduler{std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env))};
            auto start = static_cast<std::byte*>(buffer.ArrayBuffer().Data()) + buffer.ByteOffset();
            auto pixelData{std::vector<std::byte>(start, start + buffer.ByteLength())};

            arcana::make_task(arcana::threadpool_scheduler, arcana::cancellation_source::none(),
                [pixelData{std::move(pixelData)}, width, height, invertY]() {
                    return EncodePNG(pixelData, width, height, invertY);
                })
                .then(*runtimeScheduler, arcana::cancellation_source::none(),
                    [runtimeScheduler, deferred, env](const arcana::expected<std::shared_ptr<std::vector<std::byte>>, std::exception_ptr>& result) {
                        // TODO: Crash risk on JS teardown - this async work isn't tied to any JS object lifetime,
                        // unlike other plugins that cancel / clean up pending work in their destructors.
                        if (result.has_error())
                        {
                            deferred.Reject(Napi::Error::New(env, result.error()).Value());
                            return;
                        }

                        auto& imageData = result.value();
                        auto arrayBuffer{Napi::ArrayBuffer::New(env, imageData->data(), imageData->size(), [imageData](Napi::Env, void*) {})};

                        auto blobCtor = env.Global().Get("Blob").As<Napi::Function>();
                        auto blobParts = Napi::Array::New(env, 1);
                        blobParts.Set(uint32_t{0}, arrayBuffer);

                        auto options = Napi::Object::New(env);
                        options.Set("type", Napi::String::New(env, "image/png"));

                        auto blob = blobCtor.New({blobParts, options});

                        deferred.Resolve(blob);
                    });

            return deferred.Promise();
        }
    }
}

namespace Babylon::Plugins::NativeEncoding
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto native{JsRuntime::NativeObject::GetFromJavaScript(env)};
        native.Set("EncodeImageAsync", Napi::Function::New(env, EncodeImageAsync, "EncodeImageAsync"));
    }
}
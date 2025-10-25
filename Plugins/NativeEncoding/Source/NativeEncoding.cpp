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
        using EncodedImagePtr = std::unique_ptr<std::vector<uint8_t>, std::function<void(std::vector<uint8_t>*)>>;

        EncodedImagePtr EncodePNG(const std::vector<uint8_t>& pixelData, uint32_t width, uint32_t height, bool invertY)
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

            EncodedImagePtr result(
                new std::vector<uint8_t>(byteLength), 
                [](std::vector<uint8_t>* ptr) { printf("\nDestroying vector address: %p\n", ptr); delete ptr; }
            );

            std::memcpy(result->data(), memoryBlock.more(0), byteLength);

            printf("\nEncodePNG, result.data() address: %p, size: %zu, capacity: %zu\n",
                result->data(), result->size(), result->capacity());

            return result;
        }

        // TODO: Update this to Napi::Value for JSI.
        Napi::Value EncodeImageAsync(const Napi::CallbackInfo& info)
        {
            auto buffer{info[0].As<Napi::Uint8Array>()};
            auto width{info[1].As<Napi::Number>().Uint32Value()};
            auto height{info[2].As<Napi::Number>().Uint32Value()};
            auto mimeType{info[3].As<Napi::String>().Utf8Value()};
            auto invertY{info[4].As<Napi::Boolean>().Value()};
            
            auto env{info.Env()};
            auto deferred{Napi::Promise::Deferred::New(env)};
            
            if (mimeType != "image/png")
            {
                deferred.Reject(Napi::Error::New(env, "Unsupported mime type: " + mimeType + ". Only image/png is currently supported.").Value());
                return deferred.Promise();
            }

            if (buffer.ByteLength() != width * height * 4)
            {
                deferred.Reject(Napi::Error::New(env, "Buffer byte length does not match RGBA8 format (4 bytes per pixel) of provided dimensions.").Value());
                return deferred.Promise();
            }

            auto runtimeScheduler{std::make_shared<JsRuntimeScheduler>(JsRuntime::GetFromJavaScript(env))};
            auto pixelData{std::vector<uint8_t>(buffer.Data(), buffer.Data() + buffer.ByteLength())};

            arcana::make_task(arcana::threadpool_scheduler, arcana::cancellation_source::none(),
                [pixelData{std::move(pixelData)}, width, height, invertY]() {
                    return EncodePNG(pixelData, width, height, invertY);
                })
                .then(*runtimeScheduler, arcana::cancellation_source::none(),
                    /**
                        The task framework passes the stored expected as an lvalue. Don't know how it relates to below,
                        but here's what happens with different parameter types:
                        1.	By-value parameter (expected<v, e> result)            -> Copies the expected and the value v
                        2.	Rvalue reference (expected<v, e> &&result)            -> Build error
                        3.	Const lvalue reference (const expected<v, e> &result) -> Doesn't copy (both expected and value v are const refs)
                        4.  Lvalue reference (expected<v, e> &result)             -> Build error
                        5.  Anything like this (...expected<v&, e>.. result)      -> Build error

                        Options 2, 4, and 5 give build errors, so they are out.
                        And option 1 copies both the expected and the value inside, which is not ideal for performance.
                        Thus, we are left with option 3.

                        Then, there is the issue of the smart pointer inside the expected:
                        1. unique_ptr: non-copyable, movable -> Explicit ownership, but requires const_cast to move.
                        2. shared_ptr: copyable, movable -> Ambiguous ownership, but easier to use.
                    */
                    [runtimeScheduler, deferred, env](const arcana::expected<EncodedImagePtr, std::exception_ptr>& result) {
                        // TODO: Crash risk on JS teardown - this async work isn't tied to any JS object lifetime,
                        // unlike other plugins that cancel / clean up pending work in their destructors.
                        if (result.has_error())
                        {
                            deferred.Reject(Napi::Error::New(env, result.error()).Value());
                            return;
                        }

                        printf("\nContinuation, result.value().data() address: %p, size: %zu, capacity: %zu\n",
                            result.value()->data(), result.value()->size(), result.value()->capacity());

                        auto& imageData = result.value();

                        printf("\nContinuation, imageData->data() address: %p\n", imageData->data());

                        // Copy the raw pointer & size ahead of time for use in argument list (otherwise, not guaranteed to evaluate before the std::move() in lambda capture -> errors)
                        // TODO: might still need a shared_ptr here but lets see
                        auto dataPtr = imageData->data();
                        auto dataSize = imageData->size();
                        auto arrayBuffer{Napi::ArrayBuffer::New(env, dataPtr, dataSize, [imageData{std::move(const_cast<EncodedImagePtr&>(imageData))}](Napi::Env, void*) {})};
                        
                        printf("\nContinuation, arrayBuffer.Data() address: %p\n\n", arrayBuffer.Data());

                        deferred.Resolve(arrayBuffer);
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
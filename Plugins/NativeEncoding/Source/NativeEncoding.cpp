#include <Babylon/Plugins/NativeEncoding.h>
#include <Babylon/JsRuntime.h>

#include <napi/napi.h>
#include <Babylon/JsRuntime.h>
#include <Babylon/Graphics/DeviceContext.h>
#include <bimg/encode.h>
#include <bx/readerwriter.h>

namespace Babylon::Plugins
{
    namespace
    {
        Napi::ArrayBuffer EncodePNG(Napi::Env env, const uint8_t* pixelData, uint32_t width, uint32_t height, bool invertY)
        {
            bx::MemoryBlock memoryBlock(&Graphics::DeviceContext::GetDefaultAllocator());
            bx::MemoryWriter writer(&memoryBlock);
            bx::Error err;

            bimg::imageWritePng(&writer, width, height, width * 4, pixelData, bimg::TextureFormat::RGBA8, !invertY, &err);

            if (!err.isOk())
            {
                throw Napi::Error::New(env, "Failed to encode PNG image: " + std::string(err.getMessage().getCPtr()));
            }

            auto byteLength = memoryBlock.getSize();
            auto result = Napi::ArrayBuffer::New(env, byteLength);
            std::memcpy(result.Data(), memoryBlock.more(0), byteLength);

            return result;
        }

        Napi::Promise EncodeImageAsync(const Napi::CallbackInfo& info)
        {
            const auto buffer = info[0].As<Napi::Uint8Array>();
            const auto width = info[1].As<Napi::Number>().Uint32Value();
            const auto height = info[2].As<Napi::Number>().Uint32Value();
            const auto mimeType = info[3].As<Napi::String>().Utf8Value();
            const auto invertY = info[4].As<Napi::Boolean>().Value();
            
            auto env{info.Env()};
            auto deferred{Napi::Promise::Deferred::New(env)};
            auto promise{deferred.Promise()};

            if (buffer.ByteLength() != width * height * 4)
            {
                deferred.Reject(Napi::RangeError::New(env, "Buffer byte length is insufficient for RGBA8 image of provided dimensions.").Value());
                return promise;
            }

            if (mimeType == "image/png")
            {
                deferred.Resolve(EncodePNG(env, buffer.Data(), width, height, invertY));
            }
            else
            {
                deferred.Reject(Napi::Error::New(env, "Unsupported mime type: " + mimeType + ". Only image/png is currently supported.").Value());
            }

            return promise;
        }
    }
}

namespace Babylon::Plugins::NativeEncoding
{
    void BABYLON_API Initialize(Napi::Env env)
    {
        auto native = JsRuntime::NativeObject::GetFromJavaScript(env);
        native.Set("EncodeImageAsync", Napi::Function::New(env, EncodeImageAsync, "EncodeImageAsync"));
    }
}